package com.breeze.qnn.yolo

/**
 * YOLO26-pose native HTP runner。直接桥接 [yolo_pose.cpp] / [yolo_jni.cpp]：
 *   - 不走 InferenceEngine / QnnNative 抽象（那头是按 fp32 NCHW 推 CNN 设计的，
 *     跟 vendor bin 的 W8A16 uint16 张量对不上）
 *   - native 端读 bin 真实的量化编码 + 自己做 letterbox+量化，喂 uint16 NCHW
 *   - 参考 QnnYolo 项目的 QairtYoloPose 实现（SM8845 真机验证通过）
 */
class YoloNative {
    companion object {
        init {
            System.loadLibrary("qnndemo")
        }
    }

    /**
     * @param nativeLibDir app nativeLibraryDir（jniLibs 打包的 libQnn*.so 目录）
     * @param binBytes vendor yolo26n-pose.bin 的完整字节（来自 test APK assets）
     * @return 空字符串表示成功；否则为错误信息（含 SOC_MISMATCH 等异常码）
     */
    external fun nativeInit(nativeLibDir: String, binBytes: ByteArray): String

    /**
     * @param width  bitmap 宽度（像素）
     * @param height bitmap 高度（像素）
     * @param rgbBytes ARGB8888 像素（每像素 4 字节，顺序 R,G,B,A）
     * @return 检测结果 flat float 数组：[count, count × (conf, x1, y1, x2, y2, 17×3 kpt)]
     *         推理失败返回 null
     */
    external fun nativeInferRgb(width: Int, height: Int, rgbBytes: ByteArray): FloatArray?

    external fun nativeRelease()
}
