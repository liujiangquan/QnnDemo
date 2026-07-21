# =============================================================================
# setup_demo.ps1
#
# Prepare QnnDemo project dependencies from QAIRT SDK.
#
# Usage (run from project root):
#   .\docs\setup_demo.ps1
#   .\docs\setup_demo.ps1 -SdkPath "F:\AI\sdk\qairt\2.48.40.260702"
#   .\docs\setup_demo.ps1 -SkipModel
# =============================================================================
[CmdletBinding()]
param(
    [string]$SdkPath = "F:\AI\sdk\qairt\2.48.40.260702",
    [switch]$SkipModel
)

$ErrorActionPreference = "Stop"

# 脚本位于 docs/ 子目录，项目根 = docs 的父目录
$ProjectRoot = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
$AppDir      = Join-Path $ProjectRoot "app"
$CppDir      = Join-Path $AppDir "src\main\cpp"
$IncludeDir  = Join-Path $CppDir "qairt_include"
$JniLibsDir  = Join-Path $AppDir "src\main\jniLibs\arm64-v8a"
$AssetsDir   = Join-Path $AppDir "src\main\assets"

function Write-Step($msg) { Write-Host "[STEP] $msg" -ForegroundColor Cyan }
function Write-Ok($msg)   { Write-Host "  [OK] $msg" -ForegroundColor Green }
function Write-Warn2($msg){ Write-Host "  [!]  $msg" -ForegroundColor Yellow }
function Write-Err($msg)  { Write-Host "  [X]  $msg" -ForegroundColor Red }

# -----------------------------------------------------------------------------
# 0. Verify SDK path
# -----------------------------------------------------------------------------
Write-Step "Verify QAIRT SDK path: $SdkPath"
if (-not (Test-Path $SdkPath)) {
    Write-Err "SDK path not found: $SdkPath"
    Write-Host "Please specify the correct QAIRT SDK path via -SdkPath."
    exit 1
}
$SdkIncludeQairt = Join-Path $SdkPath "include\QAIRT"
$SdkIncludeQnn   = Join-Path $SdkPath "include\QNN"
$SdkLib          = Join-Path $SdkPath "lib\aarch64-android"
if (-not (Test-Path $SdkIncludeQairt)) { Write-Err "Missing include\QAIRT directory"; exit 1 }
if (-not (Test-Path $SdkIncludeQnn))   { Write-Err "Missing include\QNN directory"; exit 1 }
if (-not (Test-Path $SdkLib))     { Write-Err "Missing lib\aarch64-android directory"; exit 1 }
Write-Ok "SDK path valid"

# -----------------------------------------------------------------------------
# 1. Copy QAIRT + QNN headers
#    我们最终用 QNN Direct API，但 QAIRT 头依旧留着以便调试比较
# -----------------------------------------------------------------------------
Write-Step "Copy QAIRT + QNN headers to $IncludeDir"
if (Test-Path $IncludeDir) { Remove-Item -Recurse -Force $IncludeDir }
New-Item -ItemType Directory -Force -Path $IncludeDir | Out-Null

# QAIRT headers（flatten 掉嵌套 QAIRT/ 目录）
Copy-Item -Path $SdkIncludeQairt -Destination $IncludeDir -Recurse -Force
$Nested = Join-Path $IncludeDir "QAIRT"
if (Test-Path $Nested) {
    Get-ChildItem -Path $Nested -Recurse -File | ForEach-Object {
        $rel = $_.FullName.Substring($Nested.Length + 1)
        $dst = Join-Path $IncludeDir $rel
        $dstDir = Split-Path -Parent $dst
        if (-not (Test-Path $dstDir)) { New-Item -ItemType Directory -Force -Path $dstDir | Out-Null }
        Copy-Item $_.FullName -Destination $dst -Force
    }
    Remove-Item -Recurse -Force $Nested
}

# QNN headers（保留完整 QNN/ 目录结构，因为 QnnInterface.h 用 "QnnCommon.h" 引用同目录头）
$QnnDst = Join-Path $IncludeDir "QNN"
New-Item -ItemType Directory -Force -Path $QnnDst | Out-Null
Copy-Item -Path (Join-Path $SdkIncludeQnn "*") -Destination $QnnDst -Recurse -Force

$headerCount = (Get-ChildItem -Path $IncludeDir -Recurse -Filter *.h).Count
Write-Ok "Copied $headerCount header files"

# -----------------------------------------------------------------------------
# 2. Copy prebuilt .so libraries
# -----------------------------------------------------------------------------
Write-Step "Copy arm64-v8a prebuilt .so to $JniLibsDir"
if (-not (Test-Path $JniLibsDir)) { New-Item -ItemType Directory -Force -Path $JniLibsDir | Out-Null }

$RequiredLibs = @("libQnnSystem.so", "libQnnCpu.so")
$OptionalLibs = @(
    # GPU 后端（vendor 不提供）
    "libQnnGpu.so",
    # HTP 后端 —— 注意：SDK 版本的 libQnnHtp*/libQnnHtpV81Stub.so 会跟 vendor
    # firmware 的 signed skel 版本不匹配（AEE_ENOSUCHMOD 0x80000406）。
    # 生产环境应通过 <uses-native-library> 从 /vendor/lib64/ 加载 vendor 版本，
    # 或运行 docs/pull_vendor_htp_libs.sh 覆盖为 vendor 版本。
    "libQnnHtp.so", "libQnnHtpV81Stub.so", "libQnnHtpPrepare.so"
)

$copiedCount = 0
foreach ($lib in $RequiredLibs) {
    $src = Join-Path $SdkLib $lib
    $dst = Join-Path $JniLibsDir $lib
    if (Test-Path $src) {
        Copy-Item $src -Destination $dst -Force
        $copiedCount++
        Write-Ok "Copied $lib"
    } else {
        Write-Err "Missing required library: $lib"
        exit 1
    }
}
foreach ($lib in $OptionalLibs) {
    $src = Join-Path $SdkLib $lib
    $dst = Join-Path $JniLibsDir $lib
    if (Test-Path $src) {
        Copy-Item $src -Destination $dst -Force
        Write-Ok "Copied optional $lib"
    } else {
        Write-Warn2 "Optional lib not found, skip: $lib"
    }
}
Write-Ok "Copied $copiedCount required libraries"

# -----------------------------------------------------------------------------
# 3. Prepare DLC model
# -----------------------------------------------------------------------------
if ($SkipModel) {
    Write-Warn2 "Skipped DLC model generation (-SkipModel)"
} else {
    Write-Step "Prepare DLC model"
    if (-not (Test-Path $AssetsDir)) { New-Item -ItemType Directory -Force -Path $AssetsDir | Out-Null }

    $InceptionScript = Join-Path $SdkPath "examples\Models\InceptionV3\scripts\setup_inceptionv3_snpe.py"
    $InceptionDlcDir  = Join-Path $SdkPath "examples\Models\InceptionV3\dlc"

    $modelReady = $false
    if (Test-Path $InceptionDlcDir) {
        $existingDlc = Get-ChildItem -Path $InceptionDlcDir -Filter "*.dlc" -ErrorAction SilentlyContinue | Select-Object -First 1
        if ($existingDlc) {
            Copy-Item $existingDlc.FullName -Destination (Join-Path $AssetsDir "model.dlc") -Force
            Write-Ok "Using existing DLC: $($existingDlc.Name)"
            $modelReady = $true
        }
    }

    if (-not $modelReady -and (Test-Path $InceptionScript)) {
        Write-Host "  Trying InceptionV3 conversion script (requires Python + deps)..."
        try {
            Push-Location (Split-Path -Parent $InceptionScript)
            python $InceptionScript 2>&1 | ForEach-Object { Write-Host "    $_" }
            Pop-Location
            if (Test-Path $InceptionDlcDir) {
                $dlc = Get-ChildItem -Path $InceptionDlcDir -Filter "*.dlc" -ErrorAction SilentlyContinue | Select-Object -First 1
                if ($dlc) {
                    Copy-Item $dlc.FullName -Destination (Join-Path $AssetsDir "model.dlc") -Force
                    Write-Ok "Generated and copied DLC: $($dlc.Name)"
                    $modelReady = $true
                }
            }
        } catch {
            Write-Warn2 "Python conversion failed: $_"
        }
    }

    if (-not $modelReady) {
        Write-Warn2 "Could not auto-generate DLC model."
        Write-Host "  Please manually copy a .dlc file to: $AssetsDir\model.dlc"
        Write-Host "  Options:"
        Write-Host "    1. Use qairt-onnx-to-dlc to convert an ONNX model"
        Write-Host "    2. Use SNPE image-classifiers setup_inceptionv3.sh"
        Write-Host "    3. Copy any .dlc file from an existing project"
    }
}

# -----------------------------------------------------------------------------
# 4. Create placeholder files
# -----------------------------------------------------------------------------
$Placeholder = Join-Path $JniLibsDir ".gitkeep"
if (-not (Test-Path $Placeholder)) { New-Item -ItemType File -Path $Placeholder | Out-Null }

Write-Host ""
Write-Host "================================================" -ForegroundColor Green
Write-Ok "QnnDemo dependencies ready!"
Write-Host "  Next: open the project in Android Studio and click Build / Run." -ForegroundColor White
Write-Host "  Target device must be arm64-v8a (most modern Android phones)." -ForegroundColor White
Write-Host "================================================" -ForegroundColor Green
