package com.breeze.qnn.ui

import android.content.Context
import com.breeze.qnn.SamplingParams

class SamplingSettings(context: Context) {
    private val prefs = context.getSharedPreferences("sampling", Context.MODE_PRIVATE)
    var temperature: Float
        get() = prefs.getFloat("temperature", 0.7f)
        set(v) { prefs.edit().putFloat("temperature", v).apply() }
    var topP: Float
        get() = prefs.getFloat("topP", 0.9f)
        set(v) { prefs.edit().putFloat("topP", v).apply() }
    var maxTokens: Int
        get() = prefs.getInt("maxTokens", 256)
        set(v) { prefs.edit().putInt("maxTokens", v).apply() }

    fun toSamplingParams() = SamplingParams(temperature, topP, 40, maxTokens)
}
