#!/bin/bash
# =============================================================================
# 自动化 HTP/CPU/GPU 三后端 E2E 测试脚本
#
# 用法：
#   bash docs/run_e2e_tests.sh              # 跑全部
#   bash docs/run_e2e_tests.sh cpuBackend   # 只跑 CPU
#   bash docs/run_e2e_tests.sh htpBackend   # 只跑 HTP
# =============================================================================
set -u

export MSYS_NO_PATHCONV=1
PROJECT_ROOT="D:/workspace/AICode/qnn/QnnDemo"
LOG_DIR="E:/jira/qnn"
CLASS="com.breeze.qnn.BackendE2ETest"
DEVICE_TMP="/data/local/tmp/qnn_test"
mkdir -p "$LOG_DIR"

filter_test="${1:-}"
# llm* 前缀走 LlmE2ETest（LLM Genie chat 场景），其它走 BackendE2ETest（CNN 场景）
if [[ "$filter_test" == llm* ]]; then
    CLASS="com.breeze.qnn.LlmE2ETest"
    # "llm" 单独作 alias：跑 LlmE2ETest 全部方法
    if [ "$filter_test" = "llm" ]; then filter_test=""; fi
fi
test_arg=""
if [ -n "$filter_test" ]; then
    test_arg="-Pandroid.testInstrumentationRunnerArguments.class=$CLASS#$filter_test"
    log_name="$LOG_DIR/e2e_${filter_test}.log"
else
    test_arg="-Pandroid.testInstrumentationRunnerArguments.class=$CLASS"
    log_name="$LOG_DIR/e2e_all.log"
fi

# 1) 设备可用性检查
if ! timeout 5 adb devices | grep -q 'device$'; then
    echo "[ERR] 设备未连接。运行前请确认 'adb devices' 里有设备。"
    exit 1
fi

# 2) 平台签名 + sharedUserId=android.uid.system 后，app 直接可以访问 vendor 分区
#    下的 signed skel (/vendor/lib/rfsa/adsp/libQnnHtpV81Skel.so)。不再需要往
#    app 私有目录 push unsigned skel（那条路只适用于普通 untrusted_app 尝试
#    unsigned PD 的场景；见 docs/HTP-8845-诊断报告.md）
PKG=com.breeze.qnn
echo "[INFO] 平台签名 priv-app 模式：skel 由 vendor 提供，无需推送"

# 3) 清 logcat + 起后台采集
adb logcat -c
adb logcat -v threadtime -s QnnDemo:V InferenceEngine:V QAIRT:V QNN:V QnnDsp:V DSP:V \
    BackendE2ETest:V TestRunner:V AndroidRuntime:E *:E > "$log_name" &
LOGCAT_PID=$!
trap "kill $LOGCAT_PID 2>/dev/null || true" EXIT

# 4) 跑 Gradle 测试
cd "$PROJECT_ROOT"
./gradlew :app:connectedDebugAndroidTest $test_arg 2>&1 | tee -a "$log_name.gradle"
gradle_rc=${PIPESTATUS[0]}

# 5) 停 logcat 采集
sleep 1
kill $LOGCAT_PID 2>/dev/null || true

# 6) 汇总
echo ""
echo "==================== 汇总 ===================="
echo "gradle exit=$gradle_rc"
echo "logcat 输出：$log_name"
echo ""
echo "----- PASS/FAIL 快速定位 -----"
grep -aE '^.*BackendE2ETest.*(PASS:|SOFT FAIL:|FAIL:|>>> \[)' "$log_name" || echo "(无 BackendE2ETest 标记，测试可能未跑到)"
echo ""
echo "----- Key errors -----"
grep -aE 'QnnDsp.*<E>|Fatal|SIGSEGV|SIGABRT|loadRemoteSymbols|Failed to create context|status=[1-9]' "$log_name" | head -20
echo "==============================================="

exit $gradle_rc
