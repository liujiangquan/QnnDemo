package com.breeze.qnn.laya

import java.nio.file.Path

/**
 * HuggingFace tokenizer 封装。
 *
 * 通过自建 JNI shim（`libtokenizers_jni.so`，Rust `tokenizers` crate，见 `native/tokenizers-jni/`）
 * 加载 `tokenizer.json`，提供**不加特殊 token** 的编码与特殊 token id 解析。
 * 之所以不用 DJL：`ai.djl.huggingface:tokenizers` 的 jar 不带 android/arm64 原生库。
 *
 * 公开接口与之前一致：`encode(String):IntArray`、`tokenToId(String):Int`、`AutoCloseable`。
 */
class LayaTokenizer(tokenizerJson: Path) : AutoCloseable {
    private var handle: Long = nativeNew(tokenizerJson.toString())

    init { require(handle != 0L) { "tokenizer 加载失败: $tokenizerJson" } }

    /** 文本 -> token id（不加特殊 token） */
    fun encode(text: String): IntArray = nativeEncode(handle, text) ?: IntArray(0)

    /** 特殊 token 字面量 -> id（如 "<bos>"），未知返回 -1 */
    fun tokenToId(token: String): Int = nativeTokenToId(handle, token)

    override fun close() {
        if (handle != 0L) { nativeFree(handle); handle = 0L }
    }

    companion object {
        init { System.loadLibrary("tokenizers_jni") }

        @JvmStatic external fun nativeNew(path: String): Long
        @JvmStatic external fun nativeEncode(handle: Long, text: String): IntArray?
        @JvmStatic external fun nativeTokenToId(handle: Long, token: String): Int
        @JvmStatic external fun nativeFree(handle: Long)
    }
}
