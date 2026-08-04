package com.breeze.qnn.ner

import org.junit.Assert.assertEquals
import org.junit.Assert.assertTrue
import org.junit.Test

class EntityMergerTest {

    private fun model(text: String, s: Int, e: Int, t: EntityType = EntityType.LOC) =
        Entity(text, t, s, e, 0.9f, EntitySource.MODEL)

    private fun regex(text: String, s: Int, e: Int, t: EntityType = EntityType.ID_CARD) =
        Entity(text, t, s, e, 1.0f, EntitySource.REGEX)

    @Test fun keepsBothWhenNoOverlap() {
        val r = EntityMerger.merge(listOf(model("深圳", 0, 2)), listOf(regex("138", 10, 13)))
        assertEquals(2, r.size)
    }

    @Test fun regexWinsOnOverlap() {
        // 身份证号里的数字被模型误识别成 LOC，应保留 ID_CARD
        val r = EntityMerger.merge(
            model = listOf(model("110105", 3, 9)),
            regex = listOf(regex("11010519491231002X", 3, 21)),
        )
        assertEquals(1, r.size)
        assertEquals(EntitySource.REGEX, r[0].source)
        assertEquals(EntityType.ID_CARD, r[0].type)
    }

    @Test fun regexWinsWhenFullyContainedByModelEntity() {
        val r = EntityMerger.merge(
            model = listOf(model("很长一段", 0, 20)),
            regex = listOf(regex("138", 5, 8, EntityType.PHONE)),
        )
        assertEquals(1, r.size)
        assertEquals(EntityType.PHONE, r[0].type)
    }

    @Test fun emptyInputsYieldEmpty() {
        assertTrue(EntityMerger.merge(emptyList(), emptyList()).isEmpty())
    }

    @Test fun modelOnlyPassesThrough() {
        val r = EntityMerger.merge(listOf(model("深圳", 0, 2)), emptyList())
        assertEquals(1, r.size)
        assertEquals(EntitySource.MODEL, r[0].source)
    }

    @Test fun regexOnlyPassesThrough() {
        val r = EntityMerger.merge(emptyList(), listOf(regex("a@b.cn", 0, 6, EntityType.EMAIL)))
        assertEquals(1, r.size)
    }

    @Test fun resultIsSortedByStartThenEnd() {
        val r = EntityMerger.merge(
            model = listOf(model("丙", 20, 21), model("甲", 0, 1), model("乙", 0, 3)),
            regex = emptyList(),
        )
        assertEquals(listOf(0, 0, 20), r.map { it.charStart })
        assertEquals(listOf(1, 3, 21), r.map { it.charEnd })
    }

    @Test fun overlapsHelperIsHalfOpen() {
        // [0,2) 与 [2,4) 相邻但不重叠
        assertTrue(!model("甲", 0, 2).overlaps(model("乙", 2, 4)))
        assertTrue(model("甲", 0, 3).overlaps(model("乙", 2, 4)))
    }
}
