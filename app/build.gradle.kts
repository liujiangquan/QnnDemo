plugins {
    alias(libs.plugins.android.application)
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
                cppFlags("-std=c++17 -fexceptions -frtti -O2")
                arguments("-DANDROID_STL=c++_static")
            }
        }
    }

    // AOSP 平台签名（本分支专属）
    //
    // ../keystore/platform.keystore 由 AOSP 公开的 platform.pk8 + platform.x509.pem
    // 转换而来（build/target/product/security/），密码是 AOSP 默认的 "android"。
    // 生产环境需要跟目标设备 firmware 使用的实际 platform key 一致；这里的 keystore
    // 只有在目标设备也是 AOSP test-keys 签的（如 aosp_arm64 / aosp_car / 内部 AOSP
    // build）时才能获得 android.uid.system 权限。
    //
    // 密码同样通过 gradle.properties / 环境变量覆盖，默认走 "android"：
    //   RELEASE_STORE_PASSWORD, RELEASE_KEY_ALIAS, RELEASE_KEY_PASSWORD
    val storePwd = System.getenv("RELEASE_STORE_PASSWORD")
        ?: (project.findProperty("RELEASE_STORE_PASSWORD") as String? ?: "android")
    val keyAlias  = System.getenv("RELEASE_KEY_ALIAS")
        ?: (project.findProperty("RELEASE_KEY_ALIAS") as String? ?: "android")
    val keyPwd    = System.getenv("RELEASE_KEY_PASSWORD")
        ?: (project.findProperty("RELEASE_KEY_PASSWORD") as String? ?: "android")

    signingConfigs {
        create("platform") {
            storeFile = rootProject.file("keystore/platform.keystore")
            storePassword = storePwd
            this.keyAlias = keyAlias
            keyPassword = keyPwd
        }
    }

    buildTypes {
        debug {
            signingConfig = signingConfigs.getByName("platform")
        }
        release {
            optimization {
                enable = false
            }
            signingConfig = signingConfigs.getByName("platform")
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
