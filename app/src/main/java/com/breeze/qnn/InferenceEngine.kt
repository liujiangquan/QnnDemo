package com.breeze.qnn

import android.content.Context
import android.util.Log
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext
import java.io.File

/**
 * QAIRT 推理引擎：封装 native 调用，提供 Kotlin 友好的接口。
 *
 * 典型用法：
 *   val engine = InferenceEngine(context)
 *   engine.init()
 *   engine.loadDlc(dlcPath, QnnNative.Backend.CPU)
 *   val graphs = engine.graphInfos
 *   val result = engine.execute(graphs[0].name, inputs)
 *   engine.close()
 */
class InferenceEngine(private val context: Context) : AutoCloseable {

    private val native = QnnNative()
    private var handle: Long = 0L
    private var initialized = false
    private var loaded = false

    /** 图信息列表（loadDlc 成功后可用） */
    @Volatile
    var graphInfos: List<GraphInfo> = emptyList()
        private set

    /** 初始化 native 运行时 */
    fun init(): Boolean {
        if (handle == 0L) handle = native.nativeCreate()
        if (handle == 0L) {
            Log.e(TAG, "nativeCreate 失败")
            return false
        }
        // nativeLibraryDir 包含 jniLibs 中打包的 .so 文件
        val libDir = context.applicationInfo.nativeLibraryDir
        // HTP 后端需要 unsigned skel 从 app 私有目录读取（/data/local/tmp 上 app 读不到，
        // 只有 shell 能读）。给一个可读路径给 native，让它加进 ADSP_LIBRARY_PATH。
        val htpSkelDir = "${context.filesDir.absolutePath}/htp"
        java.io.File(htpSkelDir).mkdirs()
        val searchPath = "$libDir;$htpSkelDir"
        initialized = native.nativeInit(handle, searchPath)
        if (!initialized) Log.e(TAG, "nativeInit 失败, searchPath=$searchPath")
        else Log.i(TAG, "nativeInit 成功, searchPath=$searchPath")
        return initialized
    }

    /** 从 assets 复制 DLC 到缓存目录后加载 */
    suspend fun loadDlcFromAssets(assetName: String, backend: QnnNative.Backend): Boolean =
        withContext(Dispatchers.IO) {
            if (!initialized) {
                Log.e(TAG, "请先调用 init()")
                return@withContext false
            }
            val outFile = File(context.cacheDir, assetName.substringAfterLast('/'))
            try {
                context.assets.open(assetName).use { input ->
                    outFile.outputStream().use { output -> input.copyTo(output) }
                }
                Log.i(TAG, "DLC 已从 assets 复制到 ${outFile.absolutePath}")
            } catch (e: Exception) {
                Log.e(TAG, "从 assets 复制 DLC 失败: ${e.message}")
                return@withContext false
            }
            loadDlc(outFile.absolutePath, backend)
        }

    /** 加载 DLC 文件并构建运行时 */
    suspend fun loadDlc(dlcPath: String, backend: QnnNative.Backend): Boolean =
        withContext(Dispatchers.IO) {
            if (!initialized) {
                Log.e(TAG, "请先调用 init()")
                return@withContext false
            }
            loaded = native.nativeLoadDlc(handle, dlcPath, backend.value)
            if (loaded) {
                val json = native.nativeGetGraphInfoJson(handle)
                graphInfos = GraphInfo.listFromJson(json)
                Log.i(TAG, "DLC 加载成功, 图数量=${graphInfos.size}, backend=$backend")
            } else {
                Log.e(TAG, "DLC 加载失败: $dlcPath")
            }
            return@withContext loaded
        }

    /** 执行推理 */
    suspend fun execute(graphName: String, inputs: List<ByteArray>): InferenceResult =
        withContext(Dispatchers.IO) {
            if (!loaded) {
                return@withContext InferenceResult(graphName, 0.0, "运行时未加载模型", emptyList())
            }
            val json = native.nativeExecute(handle, graphName, inputs.toTypedArray())
            InferenceResult.fromJson(json)
        }

    /** 生成全零输入（用于功能性测试） */
    fun makeZeroInputs(graph: GraphInfo): List<ByteArray> {
        return graph.inputs.map { t ->
            ByteArray(t.totalBytes.coerceAtMost(MAX_INPUT_BYTES).toInt())
        }
    }

    /** 生成随机输入（用于功能性测试） */
    fun makeRandomInputs(graph: GraphInfo): List<ByteArray> {
        return graph.inputs.map { t ->
            val size = t.totalBytes.coerceAtMost(MAX_INPUT_BYTES).toInt()
            val arr = ByteArray(size)
            java.util.Random().nextBytes(arr)
            arr
        }
    }

    override fun close() {
        if (handle != 0L) {
            native.nativeDestroy(handle)
            handle = 0L
        }
        initialized = false
        loaded = false
        graphInfos = emptyList()
    }

    private companion object {
        private const val TAG = "InferenceEngine"
        // 限制单个输入张量最大字节数，避免内存溢出（演示用）
        const val MAX_INPUT_BYTES: Long = 64L * 1024 * 1024
    }
}
