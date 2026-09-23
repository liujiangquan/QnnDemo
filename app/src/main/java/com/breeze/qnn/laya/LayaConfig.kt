package com.breeze.qnn.laya

object LayaConfig {
    const val MAX_LEN = 512        // 固定序列长度（HTP 图输入）
    const val MAX_OPTS = 16        // 固定 option 槽位（marker 维度）
    const val HEAD_MAX_LEN = 256   // 问题头(含 option) token 预算
    const val OPT_TOKEN_CAP = 48   // 单个 option 文本最大 token 数

    const val QTYPE_CHOICE = 0
    const val QTYPE_SCORE = 1
    const val QTYPE_NOUL = 2
    const val TEMPERATURE = 1.0f   // multilingual 无温度缩放

    const val TOK_BOS = "<bos>"    // 充当 cls
    const val TOK_EOS = "<eos>"    // 充当 sep
    const val TOK_MASK = "<mask>"
    const val TOK_PAD = "<pad>"

    const val SUBDIR = "laya"
    const val BIN_NAME = "laya.bin"
    const val TOKENIZER_NAME = "tokenizer.json"
}
