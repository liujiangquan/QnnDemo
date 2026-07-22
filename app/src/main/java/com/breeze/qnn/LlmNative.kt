package com.breeze.qnn

/**
 * LLM native 接口桥接。
 *
 * 对应 C++ 层 llm_jni.cpp 中导出的 JNI 函数。
 * 所有方法均通过 long handle 引用 C++ LlmRuntime 实例。
 */
class LlmNative {
    companion object {
        init {
            System.loadLibrary("qnndemo")
        }
    }

    /** 创建 native LLM 运行时实例，返回句柄 */
    external fun nativeCreate(): Long

    /** 销毁 native LLM 运行时 */
    external fun nativeDestroy(handle: Long)

    /** 初始化：设置库搜索路径 */
    external fun nativeInit(handle: Long, libSearchPath: String): Boolean

    /** 加载 LLM 模型（传入配置 JSON 路径） */
    external fun nativeLoadModel(handle: Long, configPath: String): Boolean

    /** 查询模型是否已加载 */
    external fun nativeIsLoaded(handle: Long): Boolean

    /**
     * 执行生成
     * @param handle 运行时句柄
     * @param prompt 输入 prompt
     * @param temperature 采样温度
     * @param topP nucleus 采样阈值
     * @param topK top-k 采样
     * @param maxTokens 最大生成 token 数
     * @param seed 随机种子
     * @param callback 逐 token 回调
     * @return JSON: {elapsedMs, tokensGenerated, tokensPerSec, stoppedReason, error}
     */
    external fun nativeGenerate(
        handle: Long,
        prompt: String,
        temperature: Float,
        topP: Float,
        topK: Int,
        maxTokens: Int,
        seed: Long,
        callback: TokenCallback,
    ): String

    /** 中断正在执行的生成 */
    external fun nativeStop(handle: Long)

    /** 逐 token 回调接口 */
    interface TokenCallback {
        fun onToken(tokenText: String)
        fun onError(message: String)
    }
}
