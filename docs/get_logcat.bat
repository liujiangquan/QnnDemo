@echo off
REM 抓取 QnnDemo 相关的 logcat 日志，用于诊断 DLC 加载失败
REM 用法：手机连接 USB 调试后双击运行，或在 cmd 中执行 docs\get_logcat.bat
REM 日志输出到项目根目录的 qnndemo_logcat.txt
set "PROJ_ROOT=%~dp0.."
echo 正在抓取 logcat (tag=QnnDemo, InferenceEngine, QAIRT)...
echo 日志文件: %PROJ_ROOT%\qnndemo_logcat.txt
echo 按 Ctrl+C 停止
echo.
adb logcat -c
adb logcat -v time QnnDemo:V InferenceEngine:V QAIRT:V *:S > "%PROJ_ROOT%\qnndemo_logcat.txt" 2>&1
