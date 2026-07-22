package com.breeze.qnn.ui

import android.app.Application
import androidx.lifecycle.AndroidViewModel
import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.viewModelScope
import com.breeze.qnn.LlmBackend
import com.breeze.qnn.SamplingParams
import kotlinx.coroutines.Job
import kotlinx.coroutines.launch

class LlmViewModel(app: Application) : AndroidViewModel(app) {

    private val backend = LlmBackend(app)
    val messages = mutableListOf<ChatMessage>()
    val state = MutableLiveData<State>(State.INIT)
    val error = MutableLiveData<String?>(null)

    enum class State { INIT, LOADING, READY, GENERATING, WEIGHTS_MISSING }

    private var nextId = 0L
    private var currentJob: Job? = null

    fun initIfNeeded() {
        if (state.value != State.INIT) return
        viewModelScope.launch {
            if (!backend.init()) { state.postValue(State.WEIGHTS_MISSING); return@launch }
            if (!backend.weightsReady("qwen3-3b")) {
                state.postValue(State.WEIGHTS_MISSING); return@launch
            }
            state.postValue(State.LOADING)
            if (backend.loadModel("qwen3-3b")) state.postValue(State.READY)
            else { error.postValue("loadModel 失败"); state.postValue(State.INIT) }
        }
    }

    fun send(userText: String, params: SamplingParams,
             onTokenAppended: (Int) -> Unit,
             onComplete: (Int) -> Unit) {
        if (state.value != State.READY) return
        val userMsg = ChatMessage(nextId++, ChatMessage.Role.USER, userText)
        messages.add(userMsg)
        val assistant = ChatMessage(nextId++, ChatMessage.Role.ASSISTANT, "",
            isStreaming = true)
        messages.add(assistant)
        val assistantIdx = messages.size - 1

        state.postValue(State.GENERATING)
        currentJob = viewModelScope.launch {
            val history = buildHistoryPrompt()
            backend.generate(history, params,
                onToken = { tok ->
                    messages[assistantIdx].text += tok
                    onTokenAppended(assistantIdx)
                },
                onComplete = { stats ->
                    messages[assistantIdx].isStreaming = false
                    messages[assistantIdx].speedTokensPerSec = stats.tokensPerSec
                    onComplete(assistantIdx)
                    state.postValue(State.READY)
                },
                onError = { msg ->
                    error.postValue(msg)
                    state.postValue(State.READY)
                })
        }
    }

    fun stopCurrentGeneration() { backend.stop() }

    private fun buildHistoryPrompt(): String = buildString {
        for (m in messages) {
            when (m.role) {
                ChatMessage.Role.USER      -> append("用户：").append(m.text).append("\n")
                ChatMessage.Role.ASSISTANT -> if (m.text.isNotBlank())
                    append("助手：").append(m.text).append("\n")
            }
        }
    }.trimEnd()

    override fun onCleared() {
        backend.stop()
        backend.close()
    }
}
