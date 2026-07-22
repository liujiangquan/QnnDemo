package com.breeze.qnn.ui

data class ChatMessage(
    val id: Long,
    val role: Role,
    var text: String,
    var isStreaming: Boolean = false,
    var speedTokensPerSec: Double? = null,
) {
    enum class Role { USER, ASSISTANT }
}
