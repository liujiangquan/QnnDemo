package com.breeze.qnn

/**
 * QAIRT/QNN native 接口桥接。
 *
 * 对应 C++ 层 qnn_jni.cpp 中导出的 JNI 函数。
 * 所有方法均通过 long handle 引用 C++ QairtRuntime 实例。
 */
class QnnNative {
    companion object {
        init {
            // 加载 native 库（由 CMake 构建）
            System.loadLibrary("qnndemo")
        }
    }

    /** 后端类型枚举，与 C++ BackendType 对应 */
    enum class Backend(val value: Int) {
        CPU(0),
        GPU(1),
        HTP(3);

        companion object {
            fun fromValue(v: Int): Backend = entries.firstOrNull { it.value == v } ?: CPU
        }
    }

    /** 创建 native 运行时实例，返回句柄 */
    external fun nativeCreate(): Long

    /** 销毁 native 运行时 */
    external fun nativeDestroy(handle: Long)

    /** 初始化：加载 libQairtSystem.so 及接口表 */
    external fun nativeInit(handle: Long, libSearchPath: String): Boolean

    /** 加载 DLC 模型并构建运行时 */
    external fun nativeLoadDlc(handle: Long, dlcPath: String, backend: Int): Boolean

    /**
     * 加载预编译的 context binary（qnn-context-binary-generator 产物）。
     * 图已 finalize，加载与推理都比 DLC 快得多，但与 SoC 绑死。
     */
    external fun nativeLoadContextBinary(handle: Long, binPath: String, backend: Int): Boolean

    /** 获取所有图与张量元信息的 JSON 字符串 */
    external fun nativeGetGraphInfoJson(handle: Long): String

    /**
     * 执行一次推理
     * @param handle 运行时句柄
     * @param graphName 图名
     * @param inputs 每个输入张量的扁平字节数组
     * @return 结果 JSON：{graphName, elapsedMs, error, outputs:[{size}]}
     */
    external fun nativeExecute(handle: Long, graphName: String, inputs: Array<ByteArray>): String

    /**
     * 执行一次推理并取回**实际输出字节**（[nativeExecute] 只返回尺寸）。
     *
     * NER 之类需要读 logits 数值的场景用这个；CNN 只关心"跑通没有"用 [nativeExecute] 即可。
     * @return 每个输出张量的扁平字节数组；失败返回 null
     */
    external fun nativeExecuteWithOutput(
        handle: Long,
        graphName: String,
        inputs: Array<ByteArray>,
    ): Array<ByteArray>?
}
