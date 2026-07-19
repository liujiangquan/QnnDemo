package com.breeze.qnn

import org.json.JSONArray
import org.json.JSONObject

/**
 * 张量元信息
 */
data class TensorInfo(
    val name: String,
    val id: Long,
    val rank: Int,
    val dimensions: List<Int>,
    val dataType: String,
    val dataTypeValue: Int,
    val elementBytes: Long,
    val totalElements: Long,
    val totalBytes: Long
) {
    companion object {
        fun fromJson(o: JSONObject): TensorInfo {
            val dims = ArrayList<Int>()
            val arr = o.optJSONArray("dimensions")
            if (arr != null) for (i in 0 until arr.length()) dims.add(arr.getInt(i))
            return TensorInfo(
                name = o.optString("name"),
                id = o.optLong("id"),
                rank = o.optInt("rank"),
                dimensions = dims,
                dataType = o.optString("dataType"),
                dataTypeValue = o.optInt("dataTypeValue"),
                elementBytes = o.optLong("elementBytes"),
                totalElements = o.optLong("totalElements"),
                totalBytes = o.optLong("totalBytes")
            )
        }
    }

    /** 维度字符串，例如 "1x3x299x299" */
    fun dimString(): String = dimensions.joinToString("x") { it.toString() }
}

/**
 * 图元信息
 */
data class GraphInfo(
    val name: String,
    val inputs: List<TensorInfo>,
    val outputs: List<TensorInfo>
) {
    companion object {
        fun listFromJson(json: String): List<GraphInfo> {
            val arr = JSONArray(json)
            val list = ArrayList<GraphInfo>(arr.length())
            for (i in 0 until arr.length()) {
                val o = arr.getJSONObject(i)
                val ins = ArrayList<TensorInfo>()
                val outs = ArrayList<TensorInfo>()
                val inArr = o.optJSONArray("inputs")
                if (inArr != null) for (j in 0 until inArr.length())
                    ins.add(TensorInfo.fromJson(inArr.getJSONObject(j)))
                val outArr = o.optJSONArray("outputs")
                if (outArr != null) for (j in 0 until outArr.length())
                    outs.add(TensorInfo.fromJson(outArr.getJSONObject(j)))
                list.add(GraphInfo(o.optString("name"), ins, outs))
            }
            return list
        }
    }
}

/**
 * 推理结果
 */
data class InferenceResult(
    val graphName: String,
    val elapsedMs: Double,
    val error: String,
    val outputSizes: List<Long>
) {
    val isSuccess: Boolean get() = error.isEmpty()

    companion object {
        fun fromJson(json: String): InferenceResult {
            val o = JSONObject(json)
            val sizes = ArrayList<Long>()
            val arr = o.optJSONArray("outputs")
            if (arr != null) for (i in 0 until arr.length())
                sizes.add(arr.getJSONObject(i).optLong("size"))
            return InferenceResult(
                graphName = o.optString("graphName"),
                elapsedMs = o.optDouble("elapsedMs"),
                error = o.optString("error", ""),
                outputSizes = sizes
            )
        }
    }
}
