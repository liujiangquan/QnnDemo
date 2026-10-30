package com.breeze.qnn

data class SamplingParams(
    val temperature: Float = 0.7f,
    val topP: Float = 0.9f,
    val topK: Int = 40,
    val maxTokens: Int = 512,
    val seed: Long = 42L,
)

data class GenerateStats(
    val elapsedMs: Long,
    val tokensGenerated: Int,
    val tokensPerSec: Double,
    val stoppedReason: StopReason,
)

enum class StopReason { EOS, MAX_TOKENS, USER_CANCEL, ERROR }
