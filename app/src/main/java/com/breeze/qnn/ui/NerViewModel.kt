package com.breeze.qnn.ui

import android.app.Application
import androidx.lifecycle.AndroidViewModel
import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.viewModelScope
import com.breeze.qnn.QnnNative
import com.breeze.qnn.ner.NerBackend
import com.breeze.qnn.ner.NerResult
import kotlinx.coroutines.launch

class NerViewModel(app: Application) : AndroidViewModel(app) {

    enum class State { INIT, MODEL_MISSING, LOADING, READY, RUNNING, ERROR }

    private val backend = NerBackend(app)

    val state = MutableLiveData(State.INIT)
    val error = MutableLiveData<String?>()
    val result = MutableLiveData<NerResult?>()
    val progress = MutableLiveData<Pair<Int, Int>?>()

    /** 当前选中的后端。HTP 是默认——实测 fp32 在 HTP 上 ~34ms 且精度达标。 */
    var backendChoice: QnnNative.Backend = QnnNative.Backend.HTP
        private set

    private var initialized = false
    private var loadedBackend: QnnNative.Backend? = null

    fun initIfNeeded() {
        if (initialized) return
        initialized = true
        viewModelScope.launch {
            if (!backend.init()) {
                error.value = "NER 初始化失败（词表加载或 native init 出错）"
                state.value = State.ERROR
                return@launch
            }
            if (!backend.modelReady()) {
                state.value = State.MODEL_MISSING
                return@launch
            }
            loadModel()
        }
    }

    fun selectBackend(b: QnnNative.Backend) {
        if (b == backendChoice) return
        backendChoice = b
        // 切后端必须重新 loadDlc
        loadedBackend = null
        if (backend.modelReady()) viewModelScope.launch { loadModel() }
    }

    private suspend fun loadModel() {
        if (loadedBackend == backendChoice) return
        state.value = State.LOADING
        val ok = backend.loadModel(backendChoice)
        if (ok) {
            loadedBackend = backendChoice
            state.value = State.READY
        } else {
            error.value = "DLC 加载失败（backend=$backendChoice）"
            state.value = State.ERROR
        }
    }

    fun recognize(text: String) {
        if (text.isBlank() || state.value == State.RUNNING) return
        viewModelScope.launch {
            state.value = State.RUNNING
            progress.value = null
            val r = backend.recognize(text) { done, total ->
                progress.postValue(done to total)
            }
            result.value = r
            progress.value = null
            state.value = State.READY
        }
    }

    override fun onCleared() {
        backend.close()
        super.onCleared()
    }
}
