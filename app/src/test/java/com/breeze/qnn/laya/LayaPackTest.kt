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
}
