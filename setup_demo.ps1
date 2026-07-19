# =============================================================================
# setup_demo.ps1
#
# Prepare QnnDemo project dependencies from QAIRT SDK.
#
# Usage:
#   .\setup_demo.ps1
#   .\setup_demo.ps1 -SdkPath "F:\AI\sdk\qairt\2.48.40.260702"
#   .\setup_demo.ps1 -SkipModel
# =============================================================================
[CmdletBinding()]
param(
    [string]$SdkPath = "F:\AI\sdk\qairt\2.48.40.260702",
    [switch]$SkipModel
)

$ErrorActionPreference = "Stop"

$ProjectRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
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
$SdkInclude = Join-Path $SdkPath "include\QAIRT"
$SdkLib     = Join-Path $SdkPath "lib\aarch64-android"
if (-not (Test-Path $SdkInclude)) { Write-Err "Missing include\QAIRT directory"; exit 1 }
if (-not (Test-Path $SdkLib))     { Write-Err "Missing lib\aarch64-android directory"; exit 1 }
Write-Ok "SDK path valid"

# -----------------------------------------------------------------------------
# 1. Copy QAIRT headers
# -----------------------------------------------------------------------------
Write-Step "Copy QAIRT headers to $IncludeDir"
if (Test-Path $IncludeDir) { Remove-Item -Recurse -Force $IncludeDir }
New-Item -ItemType Directory -Force -Path $IncludeDir | Out-Null

Copy-Item -Path $SdkInclude -Destination $IncludeDir -Recurse -Force
# Flatten the nested QAIRT directory so includes work as "QairtCommon/QairtCommon.h"
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
$headerCount = (Get-ChildItem -Path $IncludeDir -Recurse -Filter *.h).Count
Write-Ok "Copied $headerCount header files"

# -----------------------------------------------------------------------------
# 2. Copy prebuilt .so libraries
# -----------------------------------------------------------------------------
Write-Step "Copy arm64-v8a prebuilt .so to $JniLibsDir"
if (-not (Test-Path $JniLibsDir)) { New-Item -ItemType Directory -Force -Path $JniLibsDir | Out-Null }

$RequiredLibs = @("libQairtSystem.so", "libQairtCpu.so")
$OptionalLibs = @(
    "libQairtGpu.so", "libQairtHtp.so",
    "libQnnSystem.so", "libQnnCpu.so", "libQnnGpu.so", "libQnnHtp.so"
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
