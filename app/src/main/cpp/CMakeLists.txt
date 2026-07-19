# =============================================================================
# QnnDemo - CMake 构建配置
#
# 通过 JNI 桥接 QAIRT (Qualcomm AI Runtime) Easy API，在 Android 上执行推理。
# QAIRT 头文件与预编译 .so 由 docs/setup_demo.ps1 从 SDK 拷贝到本工程内：
#   - 头文件:  qairt_include/   (来自 SDK 的 include/QAIRT/)
#   - 动态库:  ../jniLibs/arm64-v8a/  (来自 SDK 的 lib/aarch64-android/)
# =============================================================================

cmake_minimum_required(VERSION 3.22.1)

project("qnndemo" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QAIRT 头文件目录（由 setup 脚本拷贝到此处）
set(QAIRT_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/qairt_include")

if(NOT EXISTS "${QAIRT_INCLUDE_DIR}/QairtInterface/QairtInterface.h")
    message(WARNING
        "QAIRT 头文件未找到于 ${QAIRT_INCLUDE_DIR}\n"
        "请先运行 docs/setup_demo.ps1 完成依赖准备。\n"
        "该脚本会从 QAIRT SDK 拷贝头文件与预编译库到本工程。")
endif()

# 暴露给 JNI 代码的宏
add_definitions(-DQAIRT_EASY_API)

# ===== native 库 =====
add_library(qnndemo SHARED
    qnn_jni.cpp
    qairt_runtime.cpp
)

target_include_directories(qnndemo PRIVATE
    ${QAIRT_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 链接 Android 基础库；QAIRT .so 在运行时通过 dlopen 加载，不在此链接
target_link_libraries(qnndemo
    log
    android
    dl
)

# 导出符号可见性控制
set_target_properties(qnndemo PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
)
