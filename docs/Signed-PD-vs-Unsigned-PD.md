# Signed PD 与 Unsigned PD 完整对比

> Snapdragon Hexagon DSP 上两种 Protected Domain 的技术架构、能力差异、选型指南。
>
> 本文档整合了 QnnDemo 项目在 Meizu 22 (SM8735/V73) 和 8845 StepX (SM8845/V81) 上的实测结论，可作为在其它 Qualcomm 设备上做 QNN HTP 集成的参考。

## 目录

- [1. 概念：PD 是什么](#1-概念pd-是什么)
- [2. 核心区别](#2-核心区别)
- [3. 七维度对比表](#3-七维度对比表)
- [4. 性能与能力差异](#4-性能与能力差异)
- [5. Signed PD 独有的能力](#5-signed-pd-独有的能力)
- [6. 场景选择速查](#6-场景选择速查)
- [7. 为什么 Qualcomm 要设计两套](#7-为什么-qualcomm-要设计两套)
- [8. 常见误区](#8-常见误区)
- [附录 A：如何判断当前 ROM 支持哪种](#附录-a如何判断当前-rom-支持哪种)
- [附录 B：ROM 定制时的配置指南](#附录-brom-定制时的配置指南)

---

## 1. 概念：PD 是什么

**PD = Protected Domain**（受保护域），Hexagon DSP 上的**进程沙盒**。可以类比 Linux 的进程——DSP 内核（QuRT）给每个 PD 分配独立的虚拟地址空间、内存 quota、执行优先级，多个 PD 在同一颗 DSP 上互相隔离并发运行。

DSP 上同时存在三类 PD：

```
+-------------- Hexagon DSP (物理硬件) ----------------+
|                                                      |
|  root PD              → 系统关键任务（modem、audio） |
|  (system critical)      绝对不能崩，由 vendor 掌控   |
|                                                      |
|  signed PD            → 应用可用的高信任沙盒         |
|  (secure user PD)       加载 Qualcomm 签名的代码     |
|                                                      |
|  unsigned PD          → 应用可用的低信任沙盒         |
|  (unsecure user PD)     加载任何代码（skel）         |
|                                                      |
+------------------------------------------------------+
```

Signed PD 与 Unsigned PD 都是"给 app 用的用户 PD"，差别只在**加载什么样的 skel、给多少资源、能干什么事**。

**Skel** = "skeleton"，DSP 侧的共享库（PIL image）。它跟 ARM 侧的 stub 通过 FastRPC 通信，实现 CPU ↔ DSP 的双向 RPC 调用。

---

## 2. 核心区别

**一句话**：

- **Signed PD**：skel 必须由 Qualcomm 用 SoC-batch 私钥签名，DSP loader 加载前做**加密验证**（RSA 签名 + 证书链 + 内容 hash）
- **Unsigned PD**：不做签名验证，任何 skel 都能装

其他所有派生差异——通道、权限、资源、性能——都是从这个信任模型倒推出来的。

---

## 3. 七维度对比表

| 维度 | Signed PD | Unsigned PD |
|---|---|---|
| **skel 存放位置** | `/vendor/lib/rfsa/adsp/libQnnHtpV<N>Skel.so`（vendor 分区）+ 配套 `.cat` 签名描述文件 | APK 内 assets、`/data/local/tmp/`、`/data/user/0/<pkg>/files/htp/`——任何 `ADSP_LIBRARY_PATH` 可达路径 |
| **签名验证** | DSP fastrpc loader 在加载前 cryptographically verify，跟 firmware 里内置的证书链比对 | 无验证，直接 `mmap` + 跳转 |
| **FastRPC 通道** | `/dev/fastrpc-cdsp-secure` / `/dev/fastrpc-adsp-secure` | `/dev/fastrpc-cdsp` / `/dev/fastrpc-adsp`（非 secure 变体） |
| **caller 权限门槛** | 特权（`system` UID / `platform_app` SELinux 域），因为 `-secure` 设备通常 `crw-r--r-- system:system`，非 system group 无 ioctl 权限 | 普通 uid（第三方 app 域也行），`/dev/fastrpc-cdsp` 通常 `crw-rw-r--`，允许 system group 之外的 ioctl |
| **DSP 资源配额** | 满 VTCM（V81 上 8 MB）+ 高时钟档 + QoS 优先级 + 不被抢占 | VTCM 常被限（可能只给 4 MB 或更少）+ 时钟受限 + 可能被 signed PD 抢占 |
| **信任模型** | Qualcomm → OEM → 用户，供应链认证。skel 更新走 firmware OTA | app 自主，运行时决定装什么 skel。可以任意升级 |
| **offline cache 二进制** | 独立一份格式（内部记 `HTP_V<N>_SC<SOC>_<VTCM>MB`） | 独立一份格式，**跟 signed PD cache 不兼容**（本项目实测踩过） |

---

## 4. 性能与能力差异

### 理论差距

- **Signed PD**：DSP 侧可以拿到更大 VTCM、更高时钟档，Qualcomm 认为它"更值得信任"给资源
- **Unsigned PD**：受 quota 限制，特别是 VTCM 通常被减半，导致大层不得不 spill 到 DDR，性能腰斩甚至更差

差距一般 **10-50%**，取决于模型大小和层的 VTCM 消耗。

### 本项目实测（MobileNetV2，INT8 量化）

| 设备 | Hexagon | signed PD | unsigned PD |
|---|---|---|---|
| 8845 StepX (SM8845) | V81, 8 MB VTCM | 2.3 ms | **1.59 ms** |
| Meizu 22 (SM8735) | V73 | —（vendor 无 signed skel） | 2.69 ms |

MobileNetV2 太小（每层 VTCM 用量少），unsigned PD 也够用甚至因 profile 路径不同反而快一点。**结论：小 CNN 场景两种 PD 性能差不多。**

### LLM 场景差距会显著放大

跑 Qwen / Llama 之类大模型时：
- 单个 attention layer 的 KV cache 通常 > 4 MB
- Signed PD 拿满 8 MB VTCM，layer 完整装下 → 一次算完
- Unsigned PD 只有 4 MB VTCM，得把 layer 切两半 + DDR 中转 → 通常**慢 5-10 倍**
- 部分 Qualcomm 优化（shared weights context binary、multi-graph 编排）**只有 signed PD 才能开**

**结论：跑 LLM 几乎必走 signed PD。**

---

## 5. Signed PD 独有的能力

signed PD 除了资源配额更大，还独占以下**平台特性**（unsigned PD 完全用不到）：

| 特性 | 说明 |
|---|---|
| **SecureNPU / Sensitive Data Isolation** | 加密的模型/权重、加密的中间 tensor 不落 DDR。医疗、金融、生物特征识别场景需要 |
| **Genie LLM 框架** | Qualcomm 官方 LLM runtime（`libGenie.so`），强依赖 signed PD 才能拿满 VTCM + shared weights + KV cache 分块 |
| **Multi-graph shared weights context** | 一份权重多个 graph 共享（prefill + decode），必须 signed PD |
| **DSP-side 权重加密** | 部分 OEM 定制的权重加密方案，DSP 侧解密后不出内存 |
| **DSP QoS 保护** | signed PD 可申请 `high_priority`，DSP 保证不被其它 PD 抢占；unsigned PD 无法申请 |
| **持久化 context binary** | 系统级 context 缓存跨进程复用 |

跑 MobileNet / EfficientNet 之类简单 CNN，unsigned PD 完全够用。上面这些高级能力都是 LLM / 大模型场景才用得到。

---

## 6. 场景选择速查

| 场景 | 用哪个 |
|---|---|
| 生产手机 app 跑 MobileNet 级 CNN 加速 | signed PD（vendor 提供 signed skel） |
| **生产手机 app 跑 LLM** | **只能** signed PD（VTCM 要满） |
| 内部开发 / CI 测试，跨多种 debug 设备 | **unsigned PD**（一份 APK 装到多台 debug 机） |
| Qualcomm QRD 参考板评估 | 两个都试，对比性能 |
| 手机厂 ROM 里的自家 AI 服务 | signed PD（跟 firmware 一起签发） |
| 第三方 app 想用 NPU（且 debug ROM 允许） | unsigned PD |
| DLC → offline cache 离线编译 | **PD 类型必须匹配**（signed cache 和 unsigned cache 二进制不通用） |
| 学习 / demo 项目 | unsigned PD（不用搞平台签名，AOSP test-keys 就行） |

---

## 7. 为什么 Qualcomm 要设计两套

架构上的两难：

- **只有 signed PD**：只有 Qualcomm 签的代码能跑 DSP，第三方 AI 应用完全没法上 DSP（要 Qualcomm 逐个签名，几乎不可行），**生态死**
- **只有 unsigned PD**：任何 app 都能给 DSP 塞代码，DSP 是"多租户"的（同时跑音频/modem/AI），恶意代码可能占死 DSP 影响关键系统功能，**稳定性和电池危险**

**折中方案**：
- 关键系统任务 → root PD（vendor firmware 内置，用户看不见）
- 高优先级 AI（自家 or 认证过的 OEM 服务）→ **signed PD**（Qualcomm 认证 + OEM 白名单机制）
- 第三方 AI / 开发者 → **unsigned PD**（沙盒 + 资源限制 + 特权 caller 禁用）

Qualcomm downstream fastrpc driver 里的 "**特权 caller 不许走 unsigned PD**" 就是这个折中的实现之一——**特权身份 = 你已经拿到 signed PD 通道，不许再降级到 unsigned 去规避资源限制或安全审计**（这是 8845 上 main 分支 app 走 unsigned 会报 `err 4000` 的真实原因，跟 ROM 支不支持 unsigned PD 无关）。

---

## 8. 常见误区

以下都**不能**用来判断 ROM 支不支持 unsigned PD，都是本项目排查中踩过的坑：

| 假信号 | 实际情况 |
|---|---|
| `ro.secure=1` → "肯定不支持 unsigned" | ❌ 8845 (secure=1) 实测 unsigned PD 完全可用 |
| `ro.build.type=user` → "肯定不支持 unsigned" | ⚠️ 弱相关。生产 user build 通常允许普通 app 用 unsigned，但对特权 app 禁用 |
| fastrpc-cdsp-secure 存在 → "只能 signed PD" | ❌ 两个通道都存在很常见（本次 8845 和 Meizu 22 都两个都在） |
| vendor 有 signed skel → "肯定支持 signed PD" | ✅ 相关，但还得看 caller 权限（`sharedUserId=system` + platform 签才拿得到）|
| vendor 无 signed skel → "肯定不能用 NPU" | ❌ Meizu 22 vendor 完全没 QNN，我们靠 APK 自带 unsigned skel + unsigned PD 依然跑通 |
| 平台签 + system UID → "两条路都能走" | ❌ **反而只能走 signed PD**，unsigned 通道被 driver 强制关闭 |
| 一份 APK 靠 runtime 探测切两种 PD | ❌ `sharedUserId` 是 install-time 强绑，两种模式必须分 APK / 分 build variant |

**唯一权威判据**：从 root shell 用 SDK `qnn-net-run` + 对应 Hexagon 版本的 unsigned skel + 不带 offline cache 的 DLC（比如 fp32）跑一次。通了就是 unsigned PD 通道 OK；不通再看具体错误。

---

## 附录 A：如何判断当前 ROM 支持哪种

分 7 个层次的信号，从静态到动态：

### L1 · Build 指纹（30 秒）
```bash
adb shell 'getprop ro.build.type; getprop ro.debuggable; getprop ro.secure; getprop ro.soc.model'
```
- `user` + secure=1 + debuggable=0 → 生产 ROM，**通常只有 signed PD 对 app 开放**（unsigned 可能仍存在但普通 app 域被 SELinux 拒）
- `userdebug` / `eng` → **通常两种都开**（本项目 Meizu 22 和 8845 都是 userdebug）

### L2 · FastRPC 设备节点
```bash
adb shell 'ls -la /dev/fastrpc-*'
```
三种典型状况：

```
# 只有 signed 通道
crw-r--r-- system system /dev/fastrpc-cdsp-secure
→ 只能 signed PD

# 两个都有（本项目实测两台设备均如此）
crw-rw-r-- system system /dev/fastrpc-cdsp          ← unsigned，其他 group 可读
crw-r--r-- system system /dev/fastrpc-cdsp-secure   ← signed，需 system group
→ 两条都开，看 caller uid

# 只有非 secure，权限特别宽（eng build 常见）
crw-rw-rw- ... /dev/fastrpc-cdsp
→ 全放开
```

### L3 · SELinux 标签 + 策略
```bash
adb shell 'ls -Z /dev/fastrpc-*'
```
看 label 类型（`vendor_qdsp_device` / `vendor_xdsp_device` / `fastrpc_device` / `secure_fastrpc_device` 等），然后 `sesearch` 或从 app 侧试开+看 avc denied。

### L4 · vendor signed skel 是否存在
```bash
adb shell 'ls /vendor/lib/rfsa/adsp/libQnnHtpV*Skel.so'
```
- 有 → signed PD **数据面**完备
- 无 → 就算内核允许 signed PD 也没意义（DSP 找不到 skel）

### L5 · vendor 白名单
```bash
adb shell 'grep -h -E "libQnn|libcdsprpc|libadsprpc" /vendor/etc/public.libraries*.txt'
```
- 有 `libQnn*` → app 可以走 `<uses-native-library>` 白名单
- 无 → APK 必须自打包

### L6 · 内核 config
```bash
adb shell '
    zcat /proc/config.gz 2>/dev/null | grep -iE "FASTRPC|UNSIGNED_PD"
    find /sys/module -name "disable_unsigned_pd" 2>/dev/null
    cat /sys/module/msm_fastrpc/parameters/disable_unsigned_pd 2>/dev/null
    grep -oE "fastrpc[^ ]+" /proc/cmdline 2>/dev/null
'
```
Qualcomm downstream 内建的 driver 一般不通过 module param 控制，得看 DTS / bootargs / 编译期。

### L7 · 实测（终极判据）
用项目里的探针脚本：
```bash
bash docs/probe_pd_capability.sh
```
或手动 root shell：
```bash
adb push libQnnHtpV<N>Skel.so /data/local/tmp/
adb shell 'cd /data/local/tmp && \
  LD_LIBRARY_PATH=. \
  ADSP_LIBRARY_PATH=. \
  ./qnn-net-run --backend ./libQnnHtp.so --dlc_path ./model.dlc \
                --input_list ./input.txt'
```
- `Finished Executing Graphs` → unsigned PD 通
- `AEE_ENOSUCHMOD (0x80000406)` → stub/skel 版本不匹配（不一定是 PD 拒）
- `err 4000` → PD 通道被拒（内核 or SELinux）

---

## 附录 B：ROM 定制时的配置指南

ROM 支持哪种 PD 是**多个层次配置的合集**。要么全通、要么全断。分 7 层：

### B1 · 内核 fastrpc 驱动

Qualcomm 有两套：
- **Downstream**（`drivers/char/adsprpc.c`），一般 built-in，配置 `CONFIG_MSM_ADSPRPC=y`
- **Upstream**（`drivers/misc/fastrpc.c`），模块化，配置 `CONFIG_QCOM_FASTRPC=m`

**允许 unsigned PD 的 knob**（downstream 常见，各版本命名略有出入）：
```
CONFIG_MSM_ADSPRPC=y
# 内核命令行：
androidboot.fastrpc.disable_unsigned_pd=0     # 允许
androidboot.fastrpc.disable_unsigned_pd=1     # 禁
# sysfs（如果驱动导出）：
echo 0 > /sys/module/msm_fastrpc/parameters/disable_unsigned_pd
```

### B2 · 设备树 (DTS) — 定义 CDSP context banks
```dts
&fastrpc {
    qcom,secure-domains = <0 1 2 3>;   // signed 域列表

    /* 存在这个属性 = 全局禁 unsigned PD */
    /* qcom,disable-unsigned-pd; */

    compute-cb@1 {
        compatible = "qcom,fastrpc-compute-cb";
        reg = <1>;
        /* 无 qcom,unsigned-pd 属性 = 只跑 signed */
    };

    compute-cb@8 {
        compatible = "qcom,fastrpc-compute-cb";
        reg = <8>;
        qcom,unsigned-pd;              // 该 CB 支持 unsigned PD
    };
};
```

关键：**unsigned PD 是 per context-bank 的**——可以只给某几个 CB 开 unsigned，其他强制 signed。这样即便开发机允许 unsigned，也能限制其只能拿到有限 DSP 资源，不影响关键 signed 通道。

### B3 · init.rc — fastrpc dev 权限
`vendor/etc/init/hw/init.qcom.rc`：
```
on init
    # signed 通道 —— 只给 system group
    chmod 0640 /dev/fastrpc-cdsp-secure
    chown system system /dev/fastrpc-cdsp-secure

    # unsigned 通道 —— 生产严格 vs userdebug 宽松
    # 生产（严格）：
    #   chmod 0640 /dev/fastrpc-cdsp
    # userdebug（Meizu 22 / 8845 都这样）：
    chmod 0664 /dev/fastrpc-cdsp
    chown system system /dev/fastrpc-cdsp
```

### B4 · SELinux 策略（最关键的一层）
```te
# file_contexts
/dev/fastrpc-cdsp             u:object_r:fastrpc_device:s0
/dev/fastrpc-cdsp-secure      u:object_r:secure_fastrpc_device:s0

# 允许 platform_app 走 signed PD
allow platform_app secure_fastrpc_device:chr_file rw_file_perms;
allowxperm platform_app secure_fastrpc_device:chr_file ioctl {
    0xc0088101 0xc0388101   # FASTRPC_IOCTL_INVOKE 等
};

# userdebug 场景：允许普通 app 走 unsigned PD
allow untrusted_app_all fastrpc_device:chr_file { read write ioctl open getattr };
allowxperm untrusted_app_all fastrpc_device:chr_file ioctl { 0xc0088101 ... };

# 生产：可能完全禁
# neverallow untrusted_app fastrpc_device:chr_file *;
```

**这层没配好，前面 B1-B3 白做**。

### B5 · Vendor firmware — 装 Qualcomm signed skel
Qualcomm QSSI drop 里拿：
```mk
PRODUCT_COPY_FILES += \
    vendor/qcom/proprietary/ai-runtime/prebuilts/hexagon-v81/signed/libQnnHtpV81Skel.so:$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp/libQnnHtpV81Skel.so \
    vendor/qcom/proprietary/ai-runtime/prebuilts/hexagon-v81/signed/libqnnhtpv81.cat:$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp/libqnnhtpv81.cat
```

**Signed skel 只能 Qualcomm 签**——vendor 不能自签（除非拿到特批的私钥）。每一版 QNN SDK 对应一份 signed skel，ROM 厂商需要同步更新以跟上新版 SDK 的 stub。

### B6 · Vendor whitelist — 开放给 app namespace
`vendor/etc/public.libraries-qcom.txt`：
```
libcdsprpc.so
libadsprpc.so
libQnnSystem.so
libQnnHtp.so
libQnnHtpPrepare.so
libQnnHtpV81Stub.so
libQnnHtpV81Skel.so
```

不开则 app 想用 signed PD 只能自打包 stub / prepare。**Meizu 22 的做法**是**只开 fastrpc 通道，不开 libQnn**（第三方 app 必须自带 QNN），配套 Flyme 加固限制 QNN 加速能力只给内部认证 app。

### B7 · fastrpc HAL / fastrpcd 服务
`hardware/qcom-caf/*/fastrpc/`：
```
service vendor.fastrpcd /vendor/bin/fastrpcd
    class hal
    user system
    group system
    seclabel u:r:vendor_fastrpcd:s0
```
负责 CDSP 上电、加载 signed skel、提供 IPC 给 app。较新 SDK 把这块做进 stub 库，可能没这个服务。

### 让一个 ROM 两种都支持（AOSP 参考板 / 内部测试板配置）

**必须同时做完 7 项**：

1. Kernel：`CONFIG_MSM_ADSPRPC=y`，不加 `qcom,disable-unsigned-pd`
2. DTS：至少留一个 `compute-cb` 加 `qcom,unsigned-pd`；`qcom,secure-domains` 声明 signed 域
3. init.rc：两个 dev 都建，`fastrpc-cdsp` 权限 `0664`，`fastrpc-cdsp-secure` 权限 `0640`
4. SELinux：给 `untrusted_app` / `platform_app` 分别放开对应 label 的 ioctl（细粒度 `allowxperm`）
5. Vendor firmware：装 Qualcomm signed skel + `.cat` 文件到 `/vendor/lib/rfsa/adsp/`
6. Whitelist：`public.libraries.txt` 加 `libcdsprpc.so libadsprpc.so libQnn*.so`
7. Verified boot：debug 板 disable AVB / dm-verity，允许开发者改 vendor 分区

---

## 相关文档

- [QNN 集成完整指南](QNN集成完整指南.md) — 从零集成 QNN 到 Android app 的完整流程
- [AOSP 分支说明](AOSP分支说明.md) — 本项目 aosp 分支（走 unsigned PD）的详细说明
- [HTP-8845 诊断报告](HTP-8845-诊断报告.md) — 8845 上 signed PD 打通过程的历史记录
- `docs/probe_pd_capability.sh` — 一键探测目标设备的 PD 能力

## 文档版本

- v1.0，2026-07-21
- 基于 QnnDemo 项目在 Snapdragon 8845 (V81) + Snapdragon 8s Elite (V73) 上的实测
- QAIRT SDK 2.48.40
