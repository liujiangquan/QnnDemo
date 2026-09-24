#!/usr/bin/env bash
# 交叉编译 libtokenizers_jni.so (aarch64-linux-android) 并拷入 app/src/main/jniLibs/arm64-v8a/。
#
# 前置：
#   - rustup + `rustup target add aarch64-linux-android`
#   - 一个 **Linux** Android NDK（r26+），路径给 NDK_HOME（默认 ~/ndk/android-ndk-r26d）
#   - 主机 libclang（bindgen 用；Ubuntu: apt install libclang-dev）
#
# 用法：NDK_HOME=/path/to/android-ndk-r26d ./build-android.sh
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
NDK_HOME="${NDK_HOME:-$HOME/ndk/android-ndk-r26d}"
API="${API:-34}"
NDK_BIN="$NDK_HOME/toolchains/llvm/prebuilt/linux-x86_64/bin"
SYSROOT="$NDK_HOME/toolchains/llvm/prebuilt/linux-x86_64/sysroot"

[ -x "$NDK_BIN/aarch64-linux-android${API}-clang" ] || { echo "找不到 NDK clang: $NDK_BIN/aarch64-linux-android${API}-clang"; exit 1; }

export CC_aarch64_linux_android="$NDK_BIN/aarch64-linux-android${API}-clang"
export CXX_aarch64_linux_android="$NDK_BIN/aarch64-linux-android${API}-clang++"
export AR_aarch64_linux_android="$NDK_BIN/llvm-ar"
export CARGO_TARGET_AARCH64_LINUX_ANDROID_LINKER="$NDK_BIN/aarch64-linux-android${API}-clang"
# bindgen (onig-sys) 需要主机 libclang + 目标 sysroot
export BINDGEN_EXTRA_CLANG_ARGS="--sysroot=$SYSROOT --target=aarch64-linux-android${API}"
[ -n "${LIBCLANG_PATH:-}" ] || export LIBCLANG_PATH="$(dirname "$(ldconfig -p | awk '/libclang-[0-9]/{print $NF; exit}')" 2>/dev/null || echo /usr/lib/llvm-14/lib)"

cd "$HERE"
cargo build --release --target aarch64-linux-android

OUT="$HERE/target/aarch64-linux-android/release/libtokenizers_jni.so"
DST="$HERE/../../app/src/main/jniLibs/arm64-v8a/libtokenizers_jni.so"
mkdir -p "$(dirname "$DST")"
cp -f "$OUT" "$DST"
echo "已拷贝：$DST"
ls -la "$DST"
