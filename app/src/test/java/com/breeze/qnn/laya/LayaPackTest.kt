package com.breeze.qnn.laya

import org.junit.Assert.assertEquals
import org.junit.Test
import java.nio.ByteBuffer
import java.nio.ByteOrder

class LayaPackTest {
    @Test fun `int32 小端打包长度正确`() {
        val buf = LayaBackend.packInt32(intArrayOf(1, 2, 3), pad = 0, size = 5)
        assertEquals(5 * 4, buf.size)
        val bb = ByteBuffer.wrap(buf).order(ByteOrder.LITTLE_ENDIAN)
        assertEquals(1, bb.int); assertEquals(2, bb.int); assertEquals(3, bb.int)
        assertEquals(0, bb.int); assertEquals(0, bb.int)
    }

    @Test fun `uint8 mask 打包`() {
        val buf = LayaBackend.packMask(realCount = 2, size = 4)
        assertEquals(byteArrayOf(1, 1, 0, 0).toList(), buf.toList())
    }

    @Test fun `attention mask 前 n 个为 1`() {
        val buf = LayaBackend.packInt32Ones(n = 3, size = 5)
        val bb = ByteBuffer.wrap(buf).order(ByteOrder.LITTLE_ENDIAN)
        val got = IntArray(5) { bb.int }
        assertEquals(listOf(1, 1, 1, 0, 0), got.toList())
    }

    @Test fun `packIntsAs 宽度 1 每元素 1 字节`() {
        val buf = LayaBackend.packIntsAs(intArrayOf(1, 2, 3), pad = 0, count = 5, elementBytes = 1)
        assertEquals(5, buf.size)
        assertEquals(byteArrayOf(1, 2, 3, 0, 0).toList(), buf.toList())
    }

    @Test fun `packIntsAs 宽度 8 每元素 8 字节 long 小端`() {
        val buf = LayaBackend.packIntsAs(intArrayOf(7, -1), pad = 9, count = 3, elementBytes = 8)
        assertEquals(3 * 8, buf.size)
        val bb = ByteBuffer.wrap(buf).order(ByteOrder.LITTLE_ENDIAN)
        assertEquals(7L, bb.long)
        assertEquals(-1L, bb.long)   // Int -1 -> Long -1（符号扩展）
        assertEquals(9L, bb.long)    // pad
    }

    @Test fun `packIntsAs 宽度 4 等价 packInt32`() {
        val v = intArrayOf(10, 20, 30)
        val a = LayaBackend.packIntsAs(v, pad = 7, count = 6, elementBytes = 4)
        val b = LayaBackend.packInt32(v, pad = 7, size = 6)
        assertEquals(b.toList(), a.toList())
    }
}
