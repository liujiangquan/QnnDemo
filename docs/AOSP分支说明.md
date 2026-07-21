# AOSP 分支使用说明

本分支（`aosp`）用于在 **AOSP test-keys** 签的 Snapdragon 8845 平台设备上测试 QNN
HTP，跟 `main` 分支的核心区别只在**签名方式**：

| 分支 | 签名 | 依赖 | 适用设备 |
|---|---|---|---|
| `main` | `com.stepos.unifiedsigning.signing` 统签插件（HTTP 到内网 sign-bytes） | 内网 Nexus + 内网签名服务 | StepOS ROM 生产设备 |
| `aosp` | 本地 `keystore/platform.keystore`（AOSP 公开 test-keys，密码 `android`） | 无外网依赖 | AOSP test-keys 签的开发板 / GSI / aosp_* 版本 |

其余的（QNN Direct API 实现、jniLibs、Manifest `<uses-native-library>`、
`docs/pull_vendor_htp_libs.sh` 拉 vendor 库流程）跟 `main` 完全一致。

## Keystore 说明

- 位置：`keystore/platform.keystore`（PKCS12）
- 来源：AOSP 源码 `build/target/product/security/platform.pk8` +
  `platform.x509.pem` 转换而来
- 密码：全部默认 `android`（AOSP 惯例）
- 证书公开指纹（用来跟目标设备的 firmware key 对照）：
  ```
  DN:      CN=Android, OU=Android, O=Android, L=Mountain View, ST=California, C=US
  SHA-256: C8:A2:E9:BC:CF:59:7C:2F:B6:DC:66:BE:E2:93:FC:13:F2:FC:47:EC:77:BC:6B:2B:0D:52:C1:1F:51:19:2A:B8
  ```

**警告**：这份 keystore **不是** StepOS 生产 ROM 的 platform key，只对 AOSP
test-keys 签的设备有效。装到 StepOS ROM 上会因为签名不匹配无法获得
`android.uid.system`，HTP signed PD 打不开。

## 目标设备预检

先把设备接上 `adb`，跑一遍：

```bash
# 1) 确认设备是 AOSP test-keys 签的 —— 通常 build fingerprint 里带 "aosp"
adb shell 'getprop ro.build.fingerprint'          # 期望包含 aosp / test-keys
adb shell 'getprop ro.build.tags'                 # 期望 = test-keys

# 2) 确认 vendor 有 QNN 8845 (V81) 相关库
adb shell 'ls /vendor/lib64/libQnn*.so'
adb shell 'ls /vendor/lib/rfsa/adsp/libQnnHtpV81Skel.so'

# 3) 确认 vendor 把 QNN 库开放到 app namespace
adb shell 'grep -h -E "^libQnn|^libadsprpc|^libcdsprpc" /vendor/etc/public.libraries*.txt'
```

三项都 OK 才有可能跑通。任何一项缺失都要跟设备维护方拉齐。

## 构建 + 安装 + 跑测

```bash
# 1) 首次接新设备：拉 vendor QNN 2.x 库覆盖 jniLibs（版本对齐 skel）
bash docs/pull_vendor_htp_libs.sh

# 2) 构建（默认走 AOSP platform keystore，密码 android）
./gradlew :app:assembleDebug

# 3) 验证 APK 签名指纹（可选）
apksigner verify --print-certs app/build/outputs/apk/debug/app-debug.apk
#   期望 DN = CN=Android, OU=Android, ...

# 4) 安装
adb install -r app/build/outputs/apk/debug/app-debug.apk

# 5) 确认以 system UID 运行
adb shell 'ps -A | grep com.breeze.qnn'   # 期望 UID 显示为 system

# 6) 跑 E2E
bash docs/run_e2e_tests.sh
```

## 密码 / alias 覆盖

如果对方的 AOSP 用了非默认密码（比如 release build 通常改），走环境变量：

```bash
RELEASE_STORE_PASSWORD=xxxx \
RELEASE_KEY_ALIAS=xxxx \
RELEASE_KEY_PASSWORD=xxxx \
./gradlew :app:assembleDebug
```

或写到 `gradle.properties`（`.gitignore` 已排除个人 override 文件）：

```properties
RELEASE_STORE_PASSWORD=xxxx
RELEASE_KEY_ALIAS=xxxx
RELEASE_KEY_PASSWORD=xxxx
```

## 常见问题

**签完 install 后 `ps` 里 UID 不是 system**：keystore 跟设备 firmware 的
platform key 不匹配。让设备维护方给你他们对应的 platform.pk8/x509.pem，转成
jks 覆盖 `keystore/platform.keystore` 即可。转换命令：

```bash
openssl pkcs8 -inform DER -nocrypt -in platform.pk8 -out platform.pem
openssl pkcs12 -export -in platform.x509.pem -inkey platform.pem \
    -out platform.p12 -name android -passout pass:android
keytool -importkeystore -deststorepass android -destkeypass android \
    -destkeystore platform.keystore \
    -srckeystore platform.p12 -srcstoretype PKCS12 -srcstorepass android \
    -alias android
```

**HTP 报 `AEE_ENOSUCHMOD 0x80000406`**：99% 是 vendor 库版本没对齐（AOSP
build 用的 QNN 版本跟 jniLibs 里当前打包的不一致），重跑
`docs/pull_vendor_htp_libs.sh` 覆盖。详见 `docs/QNN集成完整指南.md` §4.2。

**HTP 报 `err 4000 loadRemoteSymbols`**：AOSP kernel 可能没像 StepOS 那样禁
unsigned PD，但你的 app 也许没拿到 signed PD 权限。检查 `getprop ro.build.tags`
是否 `test-keys`，以及 device tree 有没有 QNN sepolicy。

其它错误码见 `docs/QNN集成完整指南.md` §4.1 错误码字典。

## 从 aosp 分支切回 main

```bash
git checkout main
```

两分支之间**只有签名相关文件不同**，其余（Native 代码、jniLibs 打包、docs、
测试脚本）保持同步。修 Native 代码 / QNN 集成时优先在 `main` 上改，然后
`git merge main` 到 `aosp` 拿过来即可。
