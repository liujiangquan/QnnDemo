# =============================================================================
# convert_model.ps1
#
# 下载一个小型 ONNX 模型并转换为 QAIRT DLC 格式，拷贝到 QnnDemo 的 assets 目录。
#
# 默认使用 MobileNet v2（~14 MB ONNX，转换后 DLC 约 14 MB），适合移动端 demo。
#
# 用法（在项目根目录执行）：
#   .\docs\convert_model.ps1                                   # 默认 MobileNet v2
#   .\docs\convert_model.ps1 -ModelName squeezenet              # 用 SqueezeNet (~5 MB)
#   .\docs\convert_model.ps1 -ModelName resnet18                # 用 ResNet-18 (~45 MB)
#   .\docs\convert_model.ps1 -SdkPath "F:\AI\sdk\qairt\2.48.40.260702"
# =============================================================================
[CmdletBinding()]
param(
    [string]$SdkPath = "F:\AI\sdk\qairt\2.48.40.260702",
    [ValidateSet("mobilenetv2","squeezenet","resnet18")]
    [string]$ModelName = "mobilenetv2",
    [switch]$Quantize,
    # QAIRT SDK 的 .pyd 原生扩展实际依赖 python310.dll（包括 "Py312" 变体），
    # 因此必须使用 Python 3.10 才能正常加载。这是 SDK 的已知打包问题。
    # 请从 https://www.python.org/downloads/release/python-31011/ 下载安装 Python 3.10
    [string]$PythonPath = "D:\tools\ProgramFile\Python\Python310\python.exe"
)

$ErrorActionPreference = "Stop"

# 脚本位于 docs/ 子目录，项目根 = docs 的父目录
$ProjectRoot = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
$AssetsDir   = Join-Path $ProjectRoot "app\src\main\assets"
$WorkDir     = Join-Path $ProjectRoot "build\model_convert"

function Write-Step($msg) { Write-Host "[STEP] $msg" -ForegroundColor Cyan }
function Write-Ok($msg)   { Write-Host "  [OK] $msg" -ForegroundColor Green }
function Write-Warn2($msg){ Write-Host "  [!]  $msg" -ForegroundColor Yellow }
function Write-Err($msg)  { Write-Host "  [X]  $msg" -ForegroundColor Red }

# -----------------------------------------------------------------------------
# 模型下载源（ONNX Model Zoo 官方 release）
# -----------------------------------------------------------------------------
$ModelSources = @{
    mobilenetv2 = @{
        Url = "https://github.com/onnx/models/raw/main/validated/vision/classification/mobilenet/model/mobilenetv2-12.onnx"
        File = "mobilenetv2-12.onnx"
        Size = "14 MB"
        InputName = "input"
        InputShape = "1,3,224,224"
        OutputName = "output"
    }
    squeezenet = @{
        Url = "https://github.com/onnx/models/raw/main/validated/vision/classification/squeezenet/model/squeezenet1.1-7.onnx"
        File = "squeezenet1.1-7.onnx"
        Size = "5 MB"
        InputName = "data"
        InputShape = "1,3,224,224"
        OutputName = "squeezenet0_flatten0_reshape0"
    }
    resnet18 = @{
        Url = "https://github.com/onnx/models/raw/main/validated/vision/classification/resnet/model/resnet18-v1-7.onnx"
        File = "resnet18-v1-7.onnx"
        Size = "45 MB"
        InputName = "data"
        InputShape = "1,3,224,224"
        OutputName = "resnetv17_dense0_fwd"
    }
}

# -----------------------------------------------------------------------------
# 0. 校验 SDK
# -----------------------------------------------------------------------------
Write-Step "Verify QAIRT SDK: $SdkPath"
if (-not (Test-Path $SdkPath)) { Write-Err "SDK not found"; exit 1 }
$Converter = Join-Path $SdkPath "bin\x86_64-windows-msvc\qairt-converter"
$SnpeConverter = Join-Path $SdkPath "bin\x86_64-windows-msvc\snpe-onnx-to-dlc"
if (-not (Test-Path $Converter) -and -not (Test-Path $SnpeConverter)) {
    Write-Err "No converter found in bin\x86_64-windows-msvc"
    exit 1
}
Write-Ok "SDK valid"

# -----------------------------------------------------------------------------
# 1. 准备工作目录
# -----------------------------------------------------------------------------
Write-Step "Prepare work dir: $WorkDir"
if (-not (Test-Path $WorkDir)) { New-Item -ItemType Directory -Force -Path $WorkDir | Out-Null }
if (-not (Test-Path $AssetsDir)) { New-Item -ItemType Directory -Force -Path $AssetsDir | Out-Null }

# -----------------------------------------------------------------------------
# 2. 下载 ONNX 模型
# -----------------------------------------------------------------------------
$Model = $ModelSources[$ModelName]
$OnnxPath = Join-Path $WorkDir $Model.File
Write-Step "Download ONNX model: $ModelName ($($Model.Size))"
if (Test-Path $OnnxPath) {
    Write-Ok "Already exists, skip download: $OnnxPath"
} else {
    try {
        # 用 .NET WebClient 下载（兼容老 PowerShell）
        $wc = New-Object System.Net.WebClient
        $wc.DownloadFile($Model.Url, $OnnxPath)
        Write-Ok "Downloaded to $OnnxPath"
    } catch {
        Write-Err "Download failed: $_"
        Write-Host "  Please manually download from:" -ForegroundColor Yellow
        Write-Host "    $($Model.Url)" -ForegroundColor Yellow
        Write-Host "  And place it at: $OnnxPath" -ForegroundColor Yellow
        exit 1
    }
}

# -----------------------------------------------------------------------------
# 3. 配置 QAIRT 环境（设置 SNPE_ROOT / QNN_SDK_ROOT / PATH）
# -----------------------------------------------------------------------------
Write-Step "Setup QAIRT environment"
$EnvSetup = Join-Path $SdkPath "bin\envsetup.ps1"
if (Test-Path $EnvSetup) {
    . $EnvSetup
    Write-Ok "envsetup.ps1 sourced"
} else {
    Write-Warn2 "envsetup.ps1 not found, set env vars manually"
    $env:SNPE_SDK_ROOT = $SdkPath
    $env:QNN_SDK_ROOT = $SdkPath
    $env:PATH = "$SdkPath\bin\x86_64-windows-msvc;$env:PATH"
}

# -----------------------------------------------------------------------------
# 4. 转换 ONNX -> DLC
# -----------------------------------------------------------------------------
$DlcPath = Join-Path $WorkDir "$ModelName.dlc"
Write-Step "Convert ONNX -> DLC: $DlcPath"

# 注意：qairt-converter / snpe-onnx-to-dlc 是无扩展名的 Python 脚本，
# PowerShell 会把它当作"文档"而拒绝在管道中执行，因此必须用 python 显式调用。
# 必须设置 PYTHONIOENCODING=utf-8，否则在中文 Windows（GBK 控制台）下打印日志/help 会崩溃。
$env:PYTHONIOENCODING = "utf-8"

# 转换参数：--input_network / --output_path / --source_model_input_shape（动态输入需指定 shape）
# 注意：直接用 @("a", $b, ...) 在某些 PowerShell 版本下会得到空数组，需用 List[string] 逐个 Add
$convArgs = New-Object System.Collections.Generic.List[string]
$convArgs.Add("--input_network")
$convArgs.Add($OnnxPath)
$convArgs.Add("--output_path")
$convArgs.Add($DlcPath)
if ($Model.InputName -and $Model.InputShape) {
    $convArgs.Add("--source_model_input_shape")
    $convArgs.Add($Model.InputName)
    $convArgs.Add($Model.InputShape)
}
if ($Quantize) {
    $convArgs.Add("--quantize_full")
}

# 查找可用的 Python（QAIRT SDK 仅支持 Python 3.10 / 3.12，不支持 3.13/3.14）
$pyExe = $null
if ($PythonPath -and (Test-Path $PythonPath)) {
    $pyExe = $PythonPath
} elseif (Get-Command python -ErrorAction SilentlyContinue) {
    $pyExe = "python"
} else {
    Write-Err "Python not found. Please install Python 3.12 or specify -PythonPath."
    exit 1
}

# 校验 Python 版本（必须是 3.10 或 3.12）
# 通过临时文件捕获版本输出，避免 PowerShell 流重定向与 ErrorActionPreference=Stop 的冲突
$pyVer = ""
$tmpOut = [System.IO.Path]::GetTempFileName()
try {
    & $pyExe --version *> $tmpOut 2>&1 | Out-Null
    $pyVerRaw = (Get-Content $tmpOut -Raw -ErrorAction SilentlyContinue)
    $pyVer = "$pyVerRaw".Trim()
    if ($pyVer -match "Python\s+(\d+\.\d+)") { $pyVer = $Matches[1] }
} catch {
    $pyVer = ""
} finally {
    Remove-Item $tmpOut -ErrorAction SilentlyContinue
}
Write-Host "  Python: $pyExe (version $pyVer)"
if ($pyVer -ne "3.10" -and $pyVer -ne "3.12") {
    Write-Err "Python '$pyVer' is NOT supported by QAIRT SDK. Only Python 3.10 or 3.12 are supported."
    Write-Host "  Note: SDK's .pyd native extensions depend on python310.dll, so Python 3.10 is recommended." -ForegroundColor Yellow
    Write-Host "  Please install Python 3.10 from: https://www.python.org/downloads/release/python-31011/" -ForegroundColor Yellow
    Write-Host "  Then re-run: .\docs\convert_model.ps1 -PythonPath 'C:\path\to\Python310\python.exe'" -ForegroundColor Yellow
    exit 1
}

if (Test-Path $Converter) {
    Write-Host "  Using qairt-converter (via python)"
    & $pyExe $Converter $convArgs
    if ($LASTEXITCODE -ne 0) {
        Write-Warn2 "qairt-converter exit code: $LASTEXITCODE"
    }
} elseif (Test-Path $SnpeConverter) {
    Write-Host "  Using snpe-onnx-to-dlc (via python)"
    & $pyExe $SnpeConverter $convArgs
    if ($LASTEXITCODE -ne 0) {
        Write-Warn2 "snpe-onnx-to-dlc exit code: $LASTEXITCODE"
    }
} else {
    Write-Err "No converter found"
    exit 1
}

if (-not (Test-Path $DlcPath)) {
    Write-Err "DLC conversion failed"
    Write-Host "  Common causes:" -ForegroundColor Yellow
    Write-Host "    1. Python not in PATH (converters are Python scripts)" -ForegroundColor Yellow
    Write-Host "    2. Missing Python deps: pip install numpy protobuf onnx" -ForegroundColor Yellow
    Write-Host "    3. Run bin\check-windows-dependency.ps1 to verify" -ForegroundColor Yellow
    exit 1
}

$dlcSize = (Get-Item $DlcPath).Length
Write-Ok ("DLC generated: {0:N2} MB" -f ($dlcSize / 1MB))

# -----------------------------------------------------------------------------
# 5. 可选：量化（减小体积 + 加速 HTP 推理）
# -----------------------------------------------------------------------------
if ($Quantize) {
    Write-Step "Quantize DLC (INT8)"
    $QuantDlc = Join-Path $WorkDir "${ModelName}_quantized.dlc"
    $QuantTool = Join-Path $SdkPath "bin\x86_64-windows-msvc\snpe-dlc-quant.exe"
    if (Test-Path $QuantTool) {
        & $QuantTool --input_dlc $DlcPath --output_dlc $QuantDlc
        if ($LASTEXITCODE -eq 0 -and (Test-Path $QuantDlc)) {
            $DlcPath = $QuantDlc
            Write-Ok ("Quantized DLC: {0:N2} MB" -f ((Get-Item $QuantDlc).Length / 1MB))
        } else {
            Write-Warn2 "Quantization failed (exit=$LASTEXITCODE), use float DLC"
        }
    } else {
        Write-Warn2 "snpe-dlc-quant.exe not found, skip quantization"
    }
}

# -----------------------------------------------------------------------------
# 6. 拷贝 DLC 到 assets
# -----------------------------------------------------------------------------
$TargetDlc = Join-Path $AssetsDir "model.dlc"
Write-Step "Copy DLC to assets: $TargetDlc"
Copy-Item $DlcPath $TargetDlc -Force
Write-Ok "Done"

# -----------------------------------------------------------------------------
# 7. 生成输入元信息（供 App 调试参考）
# -----------------------------------------------------------------------------
$MetaPath = Join-Path $AssetsDir "model_meta.json"
$Meta = @{
    name = $ModelName
    onnxFile = $Model.File
    inputName = $Model.InputName
    inputShape = $Model.InputShape
    outputName = $Model.OutputName
    dlcSizeBytes = $dlcSize
    quantized = $Quantize.IsPresent
} | ConvertTo-Json -Depth 5
Set-Content -Path $MetaPath -Value $Meta -Encoding UTF8
Write-Ok "Meta written to $MetaPath"

Write-Host ""
Write-Host "================================================" -ForegroundColor Green
Write-Ok "Model ready for QnnDemo!"
Write-Host "  DLC:        $TargetDlc" -ForegroundColor White
Write-Host "  Meta:       $MetaPath" -ForegroundColor White
Write-Host "  Input:      $($Model.InputName) [$($Model.InputShape)]" -ForegroundColor White
Write-Host "  Output:     $($Model.OutputName)" -ForegroundColor White
Write-Host "  Next: rebuild app in Android Studio and run." -ForegroundColor White
Write-Host "================================================" -ForegroundColor Green
