plugins {
    alias(libs.plugins.android.application)
    // 统签插件——远程 HTTP 完成私钥运算，本地无 keystore。
    // signType 走项目根 gradle.properties 里的 unifiedsigning.signType=platform。
    // 挂上后 assembleDebug/assembleRelease 自动集成远程签名（无需手写 signingConfigs）。
    id("com.stepos.unifiedsigning.signing") version "1.0.5-alpha01-SNAPSHOT"
}

android {
    namespace = "com.breeze.qnn"
    compileSdk {
        version = release(36) {
            minorApiLevel = 1
        }
    }

    defaultConfig {
        applicationId = "com.breeze.qnn"
        minSdk = 34
        targetSdk = 36
        versionCode = 1
        versionName = "1.0"

        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"

        // 仅支持 arm64-v8a：QAIRT/QNN 预编译库只提供该 ABI
        ndk {
            abiFilters += "arm64-v8a"
        }

        // CMake 构建配置
        externalNativeBuild {
            cmake {
                // C++ 标准与异常支持
                cppFlags("-std=c++17 -fexceptions -frtti -O2")
                // 传递给 native 代码的宏定义
                arguments("-DANDROID_STL=c++_static")
            }
        }
    }

    // 统签插件接管签名，不要写原生 signingConfigs（会跟统签插件的 packageXxx
    // 任务重定向冲突，报 "in-process unified-apksig-signer failed: null"）

    buildTypes {
        release {
            optimization {
                enable = false
            }
        }
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_11
        targetCompatibility = JavaVersion.VERSION_11
    }

    // 启用 ViewBinding
    buildFeatures {
        viewBinding = true
    }

    // NDK 构建配置
    externalNativeBuild {
        cmake {
            path = file("src/main/cpp/CMakeLists.txt")
            version = "3.22.1"
        }
    }

    // 防止 jniLibs 中的预编译 .so 被 Gradle 压缩（QAIRT 库需在运行时直接 dlopen）
    packaging {
        jniLibs {
            useLegacyPackaging = true
        }
    }

    // 预编译 .so 与 DLC 模型放在 jniLibs / assets 中，不参与 lint
    lint {
        abortOnError = false
    }
}

dependencies {
    implementation(libs.androidx.appcompat)
    implementation(libs.androidx.core.ktx)
    implementation(libs.material)

    // 协程：用于后台执行推理
    implementation("org.jetbrains.kotlinx:kotlinx-coroutines-android:1.7.3")
    // Lifecycle / ViewModel
    implementation("androidx.lifecycle:lifecycle-runtime-ktx:2.6.2")
    implementation("androidx.lifecycle:lifecycle-viewmodel-ktx:2.6.2")
    implementation("androidx.activity:activity-ktx:1.8.2")

    testImplementation(libs.junit)
    androidTestImplementation(libs.androidx.espresso.core)
    androidTestImplementation(libs.androidx.junit)
}
