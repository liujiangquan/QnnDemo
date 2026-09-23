package com.breeze.qnn.laya

import ai.djl.huggingface.tokenizers.HuggingFaceTokenizer
import java.nio.file.Path

/** DJL HuggingFace tokenizer 封装：加载 tokenizer.json，提供不加特殊 token 的编码与特殊 token id 解析。 */
class LayaTokenizer(tokenizerJson: Path) : AutoCloseable {
    private val tok = HuggingFaceTokenizer.builder()
        .optTokenizerPath(tokenizerJson)
        .optAddSpecialTokens(false)
        .build()

    /** 文本 -> token id（不加特殊 token） */
    fun encode(text: String): IntArray =
        tok.encode(text).ids.map { it.toInt() }.toIntArray()

    /**
     * 特殊 token 字面量 -> id（如 "<bos>"），未知返回 -1。
     * DJL 0.30.0 未暴露 vocabulary 查询接口，改为直接编码该字面量：
     * 词表中的特殊/added token 会被整体匹配成单个 id；若结果不是恰好一个 id，视为未知返回 -1。
     */
    fun tokenToId(token: String): Int {
        val ids = tok.encode(token).ids
        return if (ids.size == 1) ids[0].toInt() else -1
    }

    override fun close() = tok.close()
}
