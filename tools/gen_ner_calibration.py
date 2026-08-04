#!/usr/bin/env python3
"""
为 bert-base-chinese-ner 的 INT8 量化生成 calibration 数据。

产出（默认写到 <model_dir>/calib/）：
  ids_NNN.raw / mask_NNN.raw / type_NNN.raw   每样本三个 int32 raw 文件，各 128 个元素
  input_list.txt                              qairt-quantizer 的 --input_list

用法（在 WSL 里跑，需要 venv 的 numpy）：
  python3 tools/gen_ner_calibration.py \
      --vocab /mnt/d/workspace/models/bert-base-chinese-ner/onnx/vocab.txt \
      --out   /mnt/d/workspace/models/bert-base-chinese-ner/calib

这里的 WordPiece 实现刻意保持简单，后续要 1:1 移植到 Kotlin
（app/src/main/java/com/breeze/qnn/ner/WordPieceTokenizer.kt）。
标定只需要"有代表性的激活分布"，不要求跟运行时 tokenizer 逐 token 一致；
真正需要严格一致的是推理路径。
"""
import argparse
import os
import unicodedata

import numpy as np

MAX_LEN = 128
PAD, UNK, CLS, SEP = "[PAD]", "[UNK]", "[CLS]", "[SEP]"

# 覆盖 PER / LOC / ORG / TIME 及无实体的中文句子。
# 长短混合，让 padding 比例也有代表性。
SENTENCES = [
    "张三于2021年3月加入阿里巴巴集团，负责杭州总部的技术工作。",
    "李四是北京大学的教授，长期在中关村从事人工智能研究。",
    "王五昨天从上海虹桥机场飞往深圳，参加腾讯的开发者大会。",
    "赵六在广州天河区的华为研究所工作了五年。",
    "刘德华和张学友将于下月在香港红磡体育馆举办演唱会。",
    "小明的家在江苏省苏州市工业园区。",
    "陈总在2023年12月31日签署了与字节跳动的合作协议。",
    "我叫小红，来自四川成都，现在在小米公司做产品经理。",
    "周杰伦的新专辑将在台北发布。",
    "孙悟空和唐僧从长安出发前往西天。",
    "上周三，百度在北京亦庄发布了新一代自动驾驶方案。",
    "中国科学院计算技术研究所位于海淀区。",
    "马云创办的阿里巴巴总部设在杭州余杭区。",
    "雷军表示小米汽车工厂在北京经济技术开发区。",
    "何女士于去年五月从武汉调往重庆分公司。",
    "京东物流在天津武清区新建了仓储中心。",
    "复旦大学和上海交通大学都在上海市。",
    "郭先生今天上午九点到达浦东国际机场。",
    "网易游戏的总部在广州，丁磊是创始人。",
    "比亚迪在西安高新区设有生产基地。",
    "林女士明年一月将前往南京出差。",
    "清华大学的姚期智教授获得了图灵奖。",
    "美团和滴滴的总部都位于北京市朝阳区。",
    "杨过与小龙女隐居在终南山。",
    "去年十月，宁德时代宣布在福建投资建厂。",
    "吴先生在深圳南山区的大疆公司担任工程师。",
    "浙江大学位于杭州市西湖区。",
    "黄小姐下周五要去青岛见客户。",
    "联想集团在北京和合肥都有研发中心。",
    "徐工机械总部在江苏徐州。",
    "今天天气不错，适合出去走走。",
    "这个方案还需要进一步讨论和完善。",
    "请把文件发送到我的邮箱。",
    "会议定在明天下午两点开始。",
    "系统正在处理您的请求，请稍候。",
    "该功能暂时不可用，我们正在修复。",
    "感谢您的反馈，我们会认真考虑。",
    "价格包含运费和税费。",
    "订单已经提交成功。",
    "密码长度至少八位，需包含字母和数字。",
    "小李和小王在中国移动的成都分公司共事，他们于2020年6月认识。",
    "招商银行深圳分行的客户经理张经理联系了我。",
    "海尔集团在青岛崂山区的工厂已投产二十年。",
    "去年春节，我和家人一起去了哈尔滨看冰灯。",
    "格力电器的董明珠出席了珠海的产业峰会。",
    "中兴通讯与中国联通在南京签署了战略合作备忘录。",
    "三一重工的梁稳根在长沙宣布新的投资计划。",
    "蔚来汽车在合肥的第二工厂将于明年三月投产。",
    "顺丰速运在深圳宝安区有大型分拣中心。",
    "同济大学的李教授研究桥梁工程三十余年。",
    "唯品会总部在广州，主要做电商业务。",
    "泸州老窖是四川泸州的白酒企业。",
    "王小波的作品在文学界影响深远。",
    "携程旅行网创立于上海，梁建章是创始人之一。",
    "长城汽车在河北保定拥有整车生产线。",
    "科大讯飞的语音识别技术源自合肥的中国科技大学。",
    "农业银行北京分行于上周完成了系统升级。",
    "小张下个月要从西安搬到郑州工作。",
    "五粮液集团位于四川宜宾。",
    "中国石化在山东胜利油田的产量保持稳定。",
]


def load_vocab(path):
    vocab = {}
    with open(path, "r", encoding="utf-8") as f:
        for idx, line in enumerate(f):
            vocab[line.rstrip("\n")] = idx
    return vocab


def is_cjk(ch):
    cp = ord(ch)
    return (
        0x4E00 <= cp <= 0x9FFF
        or 0x3400 <= cp <= 0x4DBF
        or 0x20000 <= cp <= 0x2A6DF
        or 0xF900 <= cp <= 0xFAFF
    )


def is_punct_or_symbol(ch):
    cp = ord(ch)
    if (33 <= cp <= 47) or (58 <= cp <= 64) or (91 <= cp <= 96) or (123 <= cp <= 126):
        return True
    return unicodedata.category(ch).startswith("P")


def basic_split(text):
    """把文本切成 (word, start_char) 列表。CJK 与标点各自成词，ASCII 连续段成词。"""
    out = []
    buf, buf_start = "", -1
    for i, ch in enumerate(text):
        if is_cjk(ch) or is_punct_or_symbol(ch):
            if buf:
                out.append((buf, buf_start))
                buf, buf_start = "", -1
            out.append((ch, i))
        elif ch.isspace():
            if buf:
                out.append((buf, buf_start))
                buf, buf_start = "", -1
        else:
            if not buf:
                buf_start = i
            buf += ch
    if buf:
        out.append((buf, buf_start))
    return out


def wordpiece(word, vocab):
    """greedy longest-match-first；返回 subword 列表，失败返回 [UNK]。"""
    if word in vocab:
        return [word]
    lowered = word.lower()
    if lowered in vocab:
        return [lowered]
    chars = lowered
    sub, start = [], 0
    while start < len(chars):
        end = len(chars)
        cur = None
        while start < end:
            piece = chars[start:end]
            if start > 0:
                piece = "##" + piece
            if piece in vocab:
                cur = piece
                break
            end -= 1
        if cur is None:
            return [UNK]
        sub.append(cur)
        start = end
    return sub


def encode(text, vocab):
    """→ (ids, mask, type_ids)，全部定长 MAX_LEN。"""
    tokens = [CLS]
    for word, _ in basic_split(text):
        if len(tokens) >= MAX_LEN - 1:
            break
        for piece in wordpiece(word, vocab):
            if len(tokens) >= MAX_LEN - 1:
                break
            tokens.append(piece)
    tokens.append(SEP)

    ids = [vocab.get(t, vocab[UNK]) for t in tokens]
    mask = [1] * len(ids)
    pad_id = vocab[PAD]
    while len(ids) < MAX_LEN:
        ids.append(pad_id)
        mask.append(0)
    return ids, mask, [0] * MAX_LEN


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--vocab", required=True)
    ap.add_argument("--out", required=True)
    args = ap.parse_args()

    vocab = load_vocab(args.vocab)
    for tok in (PAD, UNK, CLS, SEP):
        if tok not in vocab:
            raise SystemExit(f"vocab 缺少特殊 token: {tok}")
    print(f"vocab size = {len(vocab)}")

    os.makedirs(args.out, exist_ok=True)
    lines = []
    for i, sent in enumerate(SENTENCES):
        ids, mask, type_ids = encode(sent, vocab)
        names = {}
        for tag, arr in (("ids", ids), ("mask", mask), ("type", type_ids)):
            p = os.path.join(args.out, f"{tag}_{i:03d}.raw")
            np.asarray(arr, dtype=np.int32).tofile(p)
            names[tag] = os.path.abspath(p)
        # 顺序必须与 DLC 的 NW Input 顺序一致
        lines.append(
            f"input_ids:={names['ids']} "
            f"attention_mask:={names['mask']} "
            f"token_type_ids:={names['type']}"
        )

    list_path = os.path.join(args.out, "input_list.txt")
    with open(list_path, "w", encoding="utf-8") as f:
        f.write("\n".join(lines) + "\n")

    n_tok = sum(1 for t in encode(SENTENCES[0], vocab)[1] if t == 1)
    print(f"生成 {len(SENTENCES)} 个样本 -> {args.out}")
    print(f"input_list: {list_path}")
    print(f"样例（第 0 句有效 token 数 = {n_tok}）: {SENTENCES[0]}")


if __name__ == "__main__":
    main()
