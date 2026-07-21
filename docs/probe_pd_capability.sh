#!/bin/bash
# =============================================================================
# probe_pd_capability.sh
#
# 快速判断当前接入设备支持 signed PD 还是 unsigned PD（或都支持）。
# 输出结构化报告，最后打一个总结判定。
#
# 用法：bash docs/probe_pd_capability.sh
# =============================================================================
set -u
export MSYS_NO_PATHCONV=1

ok()   { echo "  [OK]  $*"; }
warn() { echo "  [!]   $*"; }
info() { echo "  [i]   $*"; }
sec()  { echo ""; echo "=== $* ==="; }

# --- 设备存在性 ---
if ! timeout 5 adb devices | grep -q 'device$'; then
    echo "[ERR] 没连上设备"; exit 1
fi
DEV=$(adb devices | grep -m1 'device$' | awk '{print $1}')
echo "device: $DEV"

# --- L1 Build 指纹 ---
sec "L1 build 指纹"
BUILD_TYPE=$(adb shell getprop ro.build.type 2>/dev/null | tr -d '\r')
SECURE=$(adb shell getprop ro.secure 2>/dev/null | tr -d '\r')
DEBUGGABLE=$(adb shell getprop ro.debuggable 2>/dev/null | tr -d '\r')
FINGERPRINT=$(adb shell getprop ro.build.fingerprint 2>/dev/null | tr -d '\r')
SOC=$(adb shell getprop ro.soc.model 2>/dev/null | tr -d '\r')
info "ro.build.type=$BUILD_TYPE  ro.secure=$SECURE  ro.debuggable=$DEBUGGABLE"
info "ro.soc.model=$SOC"
info "fingerprint=$FINGERPRINT"

USERDEBUG=0
[ "$BUILD_TYPE" = "userdebug" ] || [ "$BUILD_TYPE" = "eng" ] && USERDEBUG=1

# --- L2 fastrpc dev 存在性 + 权限 ---
sec "L2 fastrpc dev 权限"
FASTRPC_LS=$(adb shell 'ls -la /dev/fastrpc-* 2>&1' | tr -d '\r')
echo "$FASTRPC_LS"
HAS_CDSP=0
HAS_CDSP_SECURE=0
echo "$FASTRPC_LS" | grep -q 'fastrpc-cdsp[^-]' && HAS_CDSP=1
echo "$FASTRPC_LS" | grep -q 'fastrpc-cdsp-secure' && HAS_CDSP_SECURE=1

CDSP_WORLD_R=0
echo "$FASTRPC_LS" | awk '/fastrpc-cdsp[^-]/ {print $1}' | grep -q 'r--$\|r-.$\|rw.$' && CDSP_WORLD_R=1

# --- L3 SELinux label ---
sec "L3 SELinux label"
adb shell 'ls -Z /dev/fastrpc-* 2>&1' | tr -d '\r'

# --- L4 vendor signed skel ---
sec "L4 vendor signed skel"
VENDOR_SKEL=$(adb shell 'ls /vendor/lib/rfsa/adsp/libQnnHtpV*Skel.so 2>&1' | tr -d '\r')
if echo "$VENDOR_SKEL" | grep -q 'libQnnHtpV'; then
    ok "$VENDOR_SKEL"
    HAS_SIGNED_SKEL=1
else
    warn "vendor 无 signed QNN skel（signed PD 就算内核允许也没意义）"
    HAS_SIGNED_SKEL=0
fi

# --- L5 vendor whitelist ---
sec "L5 vendor public.libraries 白名单"
WHITELIST=$(adb shell 'grep -h -E "libQnn|libadsprpc|libcdsprpc" /vendor/etc/public.libraries*.txt 2>/dev/null' | tr -d '\r')
if [ -z "$WHITELIST" ]; then
    warn "无任何 QNN / fastrpc 白名单条目"
    HAS_WHITELIST_QNN=0
else
    echo "$WHITELIST"
    if echo "$WHITELIST" | grep -q libQnn; then
        HAS_WHITELIST_QNN=1
    else
        HAS_WHITELIST_QNN=0
    fi
fi

# --- L6 内核 config / 模块参数 ---
sec "L6 内核 fastrpc 信号"
adb shell '
    echo "--- /proc/config.gz FASTRPC 相关 ---"
    zcat /proc/config.gz 2>/dev/null | grep -iE "FASTRPC|UNSIGNED_PD" | head
    echo "--- /sys/module/*fastrpc*/parameters ---"
    find /sys/module -type d -iname "*fastrpc*" 2>/dev/null | while read d; do
        ls $d/parameters 2>/dev/null | while read p; do
            echo "$d/parameters/$p = $(cat $d/parameters/$p 2>/dev/null)"
        done
    done
    echo "--- /proc/cmdline fastrpc 参数 ---"
    grep -oE "fastrpc[^ ]+|disable_unsigned_pd[^ ]+" /proc/cmdline 2>/dev/null
' | tr -d '\r'

# --- 总结判定 ---
sec "总结判定"
echo ""
if [ $HAS_CDSP -eq 1 ] && [ $CDSP_WORLD_R -eq 1 ] && [ $USERDEBUG -eq 1 ]; then
    echo "  [Probably] unsigned PD 可用（要 SELinux 允许 app 域访问 fastrpc-cdsp）"
fi
if [ $HAS_CDSP_SECURE -eq 1 ] && [ $HAS_SIGNED_SKEL -eq 1 ] && [ $HAS_WHITELIST_QNN -eq 1 ]; then
    echo "  [Probably] signed PD 可用（需 platform 签 + sharedUserId=android.uid.system）"
fi
if [ $HAS_CDSP_SECURE -eq 1 ] && [ $HAS_SIGNED_SKEL -eq 0 ]; then
    echo "  [!] signed PD 通道存在但 vendor 无 signed skel → 实际不可用"
fi
if [ $HAS_CDSP -eq 0 ] && [ $HAS_CDSP_SECURE -eq 0 ]; then
    echo "  [FAIL] fastrpc dev 都不存在，本机不支持 Hexagon 加速"
fi
echo ""
echo "  ↑ 以上是静态推断。最终判定请用 root shell 跑 qnn-net-run 验证（详见 docs/QNN集成完整指南.md）"
