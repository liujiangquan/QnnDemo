#!/usr/bin/env bash
# 部署已验证的 256-token FP16 模型；保留旧模型备份，再原子替换。
# 用法：bash docs/setup_laya.sh [256-token bin] [tokenizer.json] [applicationId]
# 无参数时使用本机实验产物。App 会验证输入形状，拒绝旧的 512-token 模型。
set -euo pipefail
BIN="${1:-/mnt/d/workspace/models/laya/quant_20260925/laya_fp16_256.bin}"
TOK="${2:-/mnt/d/workspace/models/laya/tokenizer.json}"
APP_ID="${3:-com.breeze.qnn}"
[[ "$APP_ID" =~ ^[a-zA-Z0-9_]+(\.[a-zA-Z0-9_]+)+$ ]] || { echo "非法 applicationId" >&2; exit 1; }
[[ -r "$BIN" && -r "$TOK" ]] || { echo "模型或 tokenizer 不存在" >&2; exit 1; }
ADB="${ADB:-}"
if [[ -z "$ADB" ]]; then
    if command -v adb >/dev/null; then ADB=adb; else ADB=adb.exe; fi
fi
host_path() {
    if [[ "$ADB" == *.exe ]] && command -v wslpath >/dev/null; then
        wslpath -w "$1"
    else
        printf '%s\n' "$1"
    fi
}
DST="/data/local/tmp/laya_256_deploy"
USER_ID="$("$ADB" shell am get-current-user | tr -d '\r\n')"
[[ "$USER_ID" =~ ^[0-9]+$ ]] || { echo "无法查询 Android user" >&2; exit 1; }
APP_DIR="/data/user/${USER_ID}/${APP_ID}"
FILES_DIR="${APP_DIR}/files/laya"
STAMP="$(date +%Y%m%d_%H%M%S)"

"$ADB" shell mkdir -p "$DST"
"$ADB" push "$(host_path "$BIN")" "$DST/laya_fp16_256.bin"
"$ADB" push "$(host_path "$TOK")" "$DST/tokenizer.json"
"$ADB" shell am force-stop --user "$USER_ID" "$APP_ID"
if [[ "$("$ADB" shell id -u | tr -d '\r\n')" == 0 ]]; then
    # system shared UID 的 App 不能使用 run-as；继承其实际 UID/GID 和 SELinux 标签。
    OWNER="$("$ADB" shell stat -c '%u:%g' "$APP_DIR" | tr -d '\r\n')"
    [[ "$OWNER" =~ ^[0-9]+:[0-9]+$ ]] || { echo "App 尚未安装，无法确定属主" >&2; exit 1; }
    "$ADB" shell "set -e
        mkdir -p '$FILES_DIR'
        chown '$OWNER' '$APP_DIR/files' '$FILES_DIR'
        if [ -f '$FILES_DIR/laya.bin' ]; then cp -p '$FILES_DIR/laya.bin' '$FILES_DIR/laya.bin.pre256.$STAMP.bak'; fi
        cp '$DST/laya_fp16_256.bin' '$FILES_DIR/laya.bin.new'
        cp '$DST/tokenizer.json' '$FILES_DIR/tokenizer.json.new'
        chown '$OWNER' '$FILES_DIR/laya.bin.new' '$FILES_DIR/tokenizer.json.new'
        chmod 600 '$FILES_DIR/laya.bin.new' '$FILES_DIR/tokenizer.json.new'
        mv -f '$FILES_DIR/laya.bin.new' '$FILES_DIR/laya.bin'
        mv -f '$FILES_DIR/tokenizer.json.new' '$FILES_DIR/tokenizer.json'
        restorecon -RF '$FILES_DIR'
        ls -lh '$FILES_DIR'"
else
    [[ "$USER_ID" == 0 ]] || { echo "非 root 部署当前仅支持 Android user 0" >&2; exit 1; }
    "$ADB" shell "run-as '$APP_ID' sh -c 'set -e
        mkdir -p files/laya
        if [ -f files/laya/laya.bin ]; then cp -p files/laya/laya.bin files/laya/laya.bin.pre256.$STAMP.bak; fi
        cp $DST/laya_fp16_256.bin files/laya/laya.bin.new
        cp $DST/tokenizer.json files/laya/tokenizer.json.new
        chmod 600 files/laya/laya.bin.new files/laya/tokenizer.json.new
        mv -f files/laya/laya.bin.new files/laya/laya.bin
        mv -f files/laya/tokenizer.json.new files/laya/tokenizer.json
        ls -lh files/laya'"
fi
echo "完成：256 FP16 模型已部署；旧模型（如存在）备份为 laya.bin.pre256.$STAMP.bak"
