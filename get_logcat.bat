@echo off
REM 抓取 QnnDemo 相关的 logcat 日志，用于诊断 DLC 加载失败
REM 用法：手机连接 USB 调试后双击运行，或 cmd 中执行 get_logcat.bat
echo 正在抓取 logcat (tag=QnnDemo, InferenceEngine, QAIRT)...
echo 按 Ctrl+C 停止
echo.
adb logcat -c
adb logcat -v time QnnDemo:V InferenceEngine:V QAIRT:V *:S > qnndemo_logcat.txt 2>&1
