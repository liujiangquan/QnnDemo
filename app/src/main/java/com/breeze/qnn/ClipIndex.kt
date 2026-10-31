package com.breeze.qnn

import android.content.ContentUris
import android.content.Context
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.provider.MediaStore
import android.util.Log
import java.io.DataInputStream
import java.io.DataOutputStream
import java.io.File
import java.io.BufferedInputStream
import java.io.BufferedOutputStream
import java.io.FileInputStream
import java.io.FileOutputStream

/**
 * 本机图片的 CLIP 向量索引（增量）。
 *
 * 索引文件 files/clip/index.bin：固定长度记录，每条 = id(Long) + dateModified(Long) + 1024 × float。
 * 刷新时只对新增或修改过的图片重算向量，并丢弃已删除图片的条目。
 * 写入先落临时文件再原子替换，避免中途中断损坏索引。
 */
class ClipIndex(private val context: Context) {

    /** 一张图的索引条目。 */
    class Entry(val id: Long, val dateModified: Long, val vector: FloatArray)

    private val dir = File(context.filesDir, "clip").apply { mkdirs() }
    private val indexFile = File(dir, "index.bin")
    private val tmpFile = File(dir, "index.bin.tmp")

    /** 读取已有索引；文件不存在返回空列表。 */
    fun load(): List<Entry> {
        if (!indexFile.isFile) return emptyList()
        val out = ArrayList<Entry>()
        runCatching {
            DataInputStream(BufferedInputStream(FileInputStream(indexFile))).use { input ->
                while (true) {
                    val id = try { input.readLong() } catch (_: java.io.EOFException) { break }
                    val date = input.readLong()
                    val vec = FloatArray(ChineseClipJni.EMBEDDING_DIM) { input.readFloat() }
                    out.add(Entry(id, date, vec))
                }
            }
        }.onFailure { Log.e(TAG, "读取索引失败，将重建", it) }
        return out
    }

    /** 写入索引（原子替换）。 */
    fun save(entries: List<Entry>) {
        DataOutputStream(BufferedOutputStream(FileOutputStream(tmpFile))).use { out ->
            for (e in entries) {
                out.writeLong(e.id)
                out.writeLong(e.dateModified)
                for (x in e.vector) out.writeFloat(x)
            }
        }
        if (!tmpFile.renameTo(indexFile)) {
            // renameTo 在部分文件系统上对已存在目标失败：先删后改名
            indexFile.delete()
            tmpFile.renameTo(indexFile)
        }
    }

    /** 本机全部图片的 (id, dateModified)，按 MediaStore 枚举。 */
    fun listImages(): Map<Long, Long> {
        val result = HashMap<Long, Long>()
        val projection = arrayOf(MediaStore.Images.Media._ID, MediaStore.Images.Media.DATE_MODIFIED)
        context.contentResolver.query(
            MediaStore.Images.Media.EXTERNAL_CONTENT_URI, projection, null, null, null
        )?.use { c ->
            val idCol = c.getColumnIndexOrThrow(MediaStore.Images.Media._ID)
            val dateCol = c.getColumnIndexOrThrow(MediaStore.Images.Media.DATE_MODIFIED)
            while (c.moveToNext()) {
                result[c.getLong(idCol)] = c.getLong(dateCol)
            }
        }
        return result
    }

    /** 图片的内容 URI。 */
    fun uriOf(id: Long): android.net.Uri =
        ContentUris.withAppendedId(MediaStore.Images.Media.EXTERNAL_CONTENT_URI, id)

    /** 解码图片为 ARGB_8888（CLIP 输入需要 RGBA）；失败返回 null。 */
    fun decode(id: Long): Bitmap? = runCatching {
        context.contentResolver.openInputStream(uriOf(id))?.use { stream ->
            BitmapFactory.decodeStream(stream)
        }?.let { it.copy(Bitmap.Config.ARGB_8888, false) }
    }.getOrNull()

    /**
     * 增量刷新：对比 MediaStore 与已有索引，只重算新增或修改的图片，丢弃已删除的。
     * [embed] 为单张图的向量函数（通常是 ChineseClipModel::embedImage）。
     * [onProgress] 报告 (已处理, 需处理总数)。返回刷新后的条目列表。
     */
    fun refresh(
        embed: (Bitmap) -> FloatArray?,
        onProgress: (Int, Int) -> Unit = { _, _ -> },
    ): List<Entry> {
        val current = listImages()
        val existing = load().associateBy { it.id }

        // 保留仍存在且未修改的条目
        val kept = ArrayList<Entry>()
        val toEmbed = ArrayList<Pair<Long, Long>>()
        for ((id, date) in current) {
            val old = existing[id]
            if (old != null && old.dateModified == date) kept.add(old)
            else toEmbed.add(id to date)
        }

        val reused = kept.size
        var done = 0
        var failed = 0
        for ((id, date) in toEmbed) {
            val bmp = decode(id)
            val vec = bmp?.let(embed)
            if (vec != null) kept.add(Entry(id, date, vec)) else failed++
            done++
            onProgress(done, toEmbed.size)
        }

        save(kept)
        Log.i(TAG, "索引刷新：总 ${current.size}，复用 $reused，新算 ${done - failed}，失败 $failed")
        return kept
    }

    companion object {
        private const val TAG = "ClipIndex"
    }
}
