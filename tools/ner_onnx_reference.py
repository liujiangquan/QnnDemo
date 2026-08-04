#!/usr/bin/env python3
"""
用 onnxruntime 直接跑 ONNX 原模型，作为 DLC 的绝对参考。

用法：
  python3 tools/ner_onnx_reference.py \
      --onnx  <...>/onnx/model.onnx \
      --vocab <...>/onnx/vocab.txt \
      --testset <...>/testset \
      [--dump-dir <...>/out_onnx]

会打印每句的 argmax 标签序列与解出的实体；--dump-dir 时把 logits 存成
Result_N/logits.raw，布局与 qnn-net-run 一致，方便直接喂给 verify_ner_accuracy.py cmp。
"""
import argparse
import os
import sys

import numpy as np
import onnxruntime as ort

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from gen_ner_calibration import MAX_LEN, load_vocab  # noqa: E402
from verify_ner_accuracy import ID2LABEL, TEST_SENTENCES, decode_bio  # noqa: E402


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--onnx", required=True)
    ap.add_argument("--vocab", required=True)
    ap.add_argument("--testset", required=True)
    ap.add_argument("--dump-dir")
    args = ap.parse_args()

    vocab = load_vocab(args.vocab)
    inv_vocab = {v: k for k, v in vocab.items()}

    sess = ort.InferenceSession(args.onnx, providers=["CPUExecutionProvider"])
    print("ONNX inputs :", [(i.name, i.type, i.shape) for i in sess.get_inputs()])
    print("ONNX outputs:", [(o.name, o.type, o.shape) for o in sess.get_outputs()])
    in_type = sess.get_inputs()[0].type  # tensor(int64) or tensor(int32)
    np_dtype = np.int64 if "int64" in in_type else np.int32
    print(f"→ 用 {np_dtype.__name__} 喂输入\n")

    n = len(TEST_SENTENCES)
    for i in range(n):
        ids = np.fromfile(os.path.join(args.testset, f"ids_{i:03d}.raw"), dtype=np.int32)
        mask = np.fromfile(os.path.join(args.testset, f"mask_{i:03d}.raw"), dtype=np.int32)
        types = np.fromfile(os.path.join(args.testset, f"type_{i:03d}.raw"), dtype=np.int32)

        feed = {
            "input_ids": ids.astype(np_dtype).reshape(1, MAX_LEN),
            "attention_mask": mask.astype(np_dtype).reshape(1, MAX_LEN),
            "token_type_ids": types.astype(np_dtype).reshape(1, MAX_LEN),
        }
        logits = sess.run(["logits"], feed)[0].reshape(MAX_LEN, len(ID2LABEL))

        n_valid = int(mask.sum())
        toks = [inv_vocab.get(int(t), "?") for t in ids[:n_valid]]
        labs = [ID2LABEL[int(np.argmax(logits[j]))] for j in range(n_valid)]
        ents = decode_bio(logits, n_valid)

        print(f"[{i}] {TEST_SENTENCES[i]}   (n_valid={n_valid})")
        print("    tok :", " ".join(toks))
        print("    lab :", " ".join(labs))
        print("    实体:", [(t, "".join(toks[s:e])) for t, s, e in ents])
        print()

        if args.dump_dir:
            d = os.path.join(args.dump_dir, f"Result_{i}")
            os.makedirs(d, exist_ok=True)
            logits.astype(np.float32).tofile(os.path.join(d, "logits.raw"))

    if args.dump_dir:
        print(f"logits 已 dump 到 {args.dump_dir}（布局同 qnn-net-run）")


if __name__ == "__main__":
    main()
