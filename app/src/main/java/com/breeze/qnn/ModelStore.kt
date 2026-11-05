package com.breeze.qnn

import android.content.Context
import android.util.Log
import java.io.File

/**
 * 模型目录解析。
 *
 * 优先使用设备预置的公共目录 `/data/models`（便于复用与调试，重装 App 不丢模型），
 * 公共目录缺失时回退到 app 私有目录 `filesDir`，保证没有预置的设备仍能跑起来。
 *
 * 公共目录布局（与系统预置保持一致）：
 * ```
 * /data/models/
 * ├── chat/                 # LLM（Genie bundle：config + tokenizer + models 下的 .bin）
 * ├── asr/voice_asr/        # 流式语音识别
 * ├── asr/asr_example.wav   # 识别样例
 * └── clip/chineseclip/     # Chinese-CLIP 文搜图
 * ```
 */
object ModelStore {

    const val SHARED_ROOT = "/data/models"

    /** 当前演示使用的 LLM，对应 `/data/models/chat/qwen3-4b-htp-v81.json`。 */
    const val DEFAULT_LLM_ID = "qwen3-4b"

    /** 解析结果。[shared] 表示来自公共目录，便于日志区分。 */
    data class Source(val dir: File, val shared: Boolean)

    /** LLM 模型目录（含 Genie config 的目录）。 */
    fun llm(context: Context, modelId: String): Source? = pick(
        Source(File(SHARED_ROOT, "llm/$modelId"), true),
        Source(File(SHARED_ROOT, "chat"), true),
        Source(File(context.filesDir, "llm/$modelId"), false),
    ) { dir -> llmConfig(dir, modelId) != null }

    /** LLM 的 Genie config：优先 `<modelId>-htp*.json`，否则目录内第一个 `*htp*.json`。 */
    fun llmConfig(dir: File, modelId: String): File? {
        val files = dir.listFiles { f -> f.isFile && f.name.endsWith(".json") && f.name.contains("htp") }
            ?.sortedBy { it.name } ?: return null
        return files.firstOrNull { it.name.startsWith("$modelId-htp") } ?: files.firstOrNull()
    }

    /** 语音识别模型目录（含 model_config.json）。 */
    fun asr(context: Context): Source? = pick(
        Source(File(SHARED_ROOT, "asr/voice_asr"), true),
        Source(File(context.filesDir, "asr/voice_asr"), false),
    ) { dir -> File(dir, "model_config.json").isFile }

    /** 语音识别样例 wav。 */
    fun asrSample(context: Context): File? {
        val candidates = listOf(
            File(SHARED_ROOT, "asr/asr_example.wav"),
            File(context.filesDir, "asr/asr_example.wav"),
        )
        return candidates.firstOrNull { it.isFile }
    }

    /** Chinese-CLIP 模型目录（含 bundle_config.json 与两个 .mnn）。 */
    fun clip(context: Context): Source? = pick(
        Source(File(SHARED_ROOT, "clip/chineseclip"), true),
        Source(File(context.filesDir, "clip/chineseclip"), false),
    ) { dir -> dir.isDirectory }

    /** 把相对路径按模型目录补成绝对路径；已是绝对路径则原样返回。 */
    fun absolutePath(path: String, baseDir: File): String =
        if (File(path).isAbsolute) path else File(baseDir, path).absolutePath

    private fun pick(vararg candidates: Source, accept: (File) -> Boolean): Source? =
        candidates.firstOrNull { accept(it.dir) }?.also {
            Log.i(TAG, "模型目录: ${it.dir.absolutePath}（${if (it.shared) "公共" else "私有"}）")
        }

    private const val TAG = "ModelStore"
}
