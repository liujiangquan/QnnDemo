#!/usr/bin/env bash
# 把 laya.bin 与 tokenizer.json 推到 App 私有目录 filesDir/laya/
# 用法：./setup_laya.sh <laya.bin 路径> <tokenizer.json 路径> [applicationId]
set -euo pipefail
BIN="${1:?laya.bin 路径}"
TOK="${2:?tokenizer.json 路径}"
APP_ID="${3:-com.breeze.qnn}"
DST="/data/local/tmp/laya"
FILES_DIR="/data/data/${APP_ID}/files/laya"

adb shell mkdir -p "$DST"
adb push "$BIN" "$DST/laya.bin"
adb push "$TOK" "$DST/tokenizer.json"
# 经由 run-as 拷进 app 私有目录（需 debuggable）
adb shell "run-as $APP_ID mkdir -p files/laya"
adb shell "run-as $APP_ID sh -c 'cat $DST/laya.bin > files/laya/laya.bin'"
adb shell "run-as $APP_ID sh -c 'cat $DST/tokenizer.json > files/laya/tokenizer.json'"
adb shell "run-as $APP_ID sh -c 'ls -l files/laya'"
echo "完成：laya.bin + tokenizer.json 已就绪于 filesDir/laya/"
