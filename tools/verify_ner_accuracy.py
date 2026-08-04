#!/usr/bin/env python3
"""
NER DLC 精度验证：比 fp32 与 INT8（或设备 HTP）输出的 logits 余弦相似度 +
BIO 解码后的实体是否一致。

分两步用：

  # 1) 生成测试输入（用不在 calibration 集里的句子）
  python3 tools/verify_ner_accuracy.py gen \
      --vocab <...>/onnx/vocab.txt --out <...>/testset

  # 2) 各自跑 qnn-net-run 拿到 output 目录后比较
  python3 tools/verify_ner_accuracy.py cmp \
      --ref <...>/out_fp32 --test <...>/out_int8 --testset <...>/testset

门槛：余弦相似度 >= 0.99 且实体集合完全一致（见 spec 第 5 节）。
"""
import argparse
import glob
import os
import sys

import numpy as np

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from gen_ner_calibration import MAX_LEN, encode, load_vocab  # noqa: E402

# 顺序来自 onnx/config.json 的 id2label，不可改
ID2LABEL = [
    "I-ORG", "B-LOC", "O", "B-ORG", "I-LOC",
    "I-PER", "B-TIME", "I-TIME", "B-PER",
]

# 刻意与 gen_ner_calibration.SENTENCES 无交集
TEST_SENTENCES = [
    "马化腾在深圳创办了腾讯公司。",
    "钱七于2022年8月前往苏州参加华为的技术峰会。",
    "南开大学位于天津市南开区。",
    "昨天下午，格兰仕在佛山发布了新产品。",
    "郑先生下周要去西藏拉萨出差。",
    "这份报告需要在周五之前提交。",
    "中国银行上海分行的李行长接待了来访客户。",
    "小芳和小刚都在长沙的中联重科工作。",
]


def cosine(a, b):
    a, b = a.ravel().astype(np.float64), b.ravel().astype(np.float64)
    na, nb = np.linalg.norm(a), np.linalg.norm(b)
    if na == 0 or nb == 0:
        return 0.0
    return float(np.dot(a, b) / (na * nb))


def decode_bio(logits, n_valid):
    """logits: [MAX_LEN, 9] -> [(type, tokenStart, tokenEndExclusive)]，跳过 CLS/SEP/PAD。"""
    labels = [ID2LABEL[int(np.argmax(logits[i]))] for i in range(MAX_LEN)]
    ents, cur_type, cur_start = [], None, -1
    # 位置 0 是 [CLS]，n_valid-1 是 [SEP]，只看 1..n_valid-2
    for i in range(1, max(1, n_valid - 1)):
        lab = labels[i]
        if lab == "O":
            if cur_type:
                ents.append((cur_type, cur_start, i))
                cur_type, cur_start = None, -1
            continue
        prefix, _, etype = lab.partition("-")
        if prefix == "B" or cur_type != etype:
            if cur_type:
                ents.append((cur_type, cur_start, i))
            cur_type, cur_start = etype, i
    if cur_type:
        ents.append((cur_type, cur_start, max(1, n_valid - 1)))
    return ents


def cmd_gen(args):
    vocab = load_vocab(args.vocab)
    os.makedirs(args.out, exist_ok=True)
    lines, meta = [], []
    for i, sent in enumerate(TEST_SENTENCES):
        ids, mask, type_ids = encode(sent, vocab)
        paths = {}
        for tag, arr in (("ids", ids), ("mask", mask), ("type", type_ids)):
            p = os.path.abspath(os.path.join(args.out, f"{tag}_{i:03d}.raw"))
            np.asarray(arr, dtype=np.int32).tofile(p)
            paths[tag] = p
        lines.append(
            f"input_ids:={paths['ids']} "
            f"attention_mask:={paths['mask']} "
            f"token_type_ids:={paths['type']}"
        )
        meta.append(f"{int(sum(mask))}\t{sent}")

    with open(os.path.join(args.out, "input_list.txt"), "w", encoding="utf-8") as f:
        f.write("\n".join(lines) + "\n")
    with open(os.path.join(args.out, "meta.tsv"), "w", encoding="utf-8") as f:
        f.write("\n".join(meta) + "\n")
    print(f"生成 {len(TEST_SENTENCES)} 条测试输入 -> {args.out}")


def find_outputs(root):
    """qnn-net-run 输出布局：<root>/Result_N/<tensor>.raw"""
    hits = sorted(glob.glob(os.path.join(root, "Result_*", "*.raw")))
    if not hits:
        hits = sorted(glob.glob(os.path.join(root, "**", "*.raw"), recursive=True))
    return hits


def cmd_cmp(args):
    meta_path = os.path.join(args.testset, "meta.tsv")
    with open(meta_path, encoding="utf-8") as f:
        meta = [l.rstrip("\n").split("\t", 1) for l in f if l.strip()]

    ref_files, test_files = find_outputs(args.ref), find_outputs(args.test)
    if len(ref_files) != len(test_files):
        print(f"[ERR] 输出数量不一致: ref={len(ref_files)} test={len(test_files)}")
        return 2
    if not ref_files:
        print(f"[ERR] 在 {args.ref} 下找不到 .raw 输出")
        return 2

    worst, mismatch = 1.0, 0
    print(f"{'#':>3} {'cos':>8}  {'实体一致':<8} 句子")
    for i, (rf, tf) in enumerate(zip(ref_files, test_files)):
        ref = np.fromfile(rf, dtype=np.float32).reshape(MAX_LEN, len(ID2LABEL))
        tst = np.fromfile(tf, dtype=np.float32).reshape(MAX_LEN, len(ID2LABEL))
        c = cosine(ref, tst)
        worst = min(worst, c)

        n_valid = int(meta[i][0]) if i < len(meta) else MAX_LEN
        sent = meta[i][1] if i < len(meta) else "?"
        e_ref, e_tst = decode_bio(ref, n_valid), decode_bio(tst, n_valid)
        same = e_ref == e_tst
        if not same:
            mismatch += 1
        print(f"{i:>3} {c:>8.5f}  {'OK' if same else 'DIFF':<8} {sent}")
        if not same:
            print(f"      ref : {e_ref}")
            print(f"      test: {e_tst}")

    print()
    print(f"最低余弦相似度 = {worst:.5f}   (门槛 >= 0.99)")
    print(f"实体不一致条数 = {mismatch} / {len(ref_files)}   (门槛 = 0)")
    ok = worst >= 0.99 and mismatch == 0
    print("结论:", "PASS" if ok else "FAIL")
    return 0 if ok else 1


def main():
    ap = argparse.ArgumentParser()
    sub = ap.add_subparsers(dest="cmd", required=True)
    g = sub.add_parser("gen")
    g.add_argument("--vocab", required=True)
    g.add_argument("--out", required=True)
    c = sub.add_parser("cmp")
    c.add_argument("--ref", required=True)
    c.add_argument("--test", required=True)
    c.add_argument("--testset", required=True)
    args = ap.parse_args()
    sys.exit(cmd_gen(args) or 0 if args.cmd == "gen" else cmd_cmp(args))


if __name__ == "__main__":
    main()
