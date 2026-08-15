# 专注久坐提醒器 - Windows 本地构建脚本
# 在 Windows 10/11 上，以管理员权限打开 PowerShell 后执行

$ErrorActionPreference = "Stop"

Write-Host "========================================="
Write-Host "专注久坐提醒器 Windows 构建脚本 v0.1"
Write-Host "========================================="

# 1. 检查 Node.js
$nodeVersion = node --version 2>$null
if (-not $nodeVersion) {
    Write-Error "未检测到 Node.js，请先安装 LTS 版本：https://nodejs.org/"
    exit 1
}
Write-Host "Node.js 版本: $nodeVersion"

# 2. 检查 npm
$npmVersion = npm --version 2>$null
if (-not $npmVersion) {
    Write-Error "未检测到 npm"
    exit 1
}
Write-Host "npm 版本: $npmVersion"

# 3. 安装依赖
Write-Host ""
Write-Host "[1/4] 安装 npm 依赖..."
npm install
if ($LASTEXITCODE -ne 0) {
    Write-Error "npm install 失败"
    exit 1
}

# 4. 构建 Windows NSIS 安装包
Write-Host ""
Write-Host "[2/4] 构建 Windows NSIS 安装包..."
npm run dist:win
if ($LASTEXITCODE -ne 0) {
    Write-Error "构建失败"
    exit 1
}

# 5. 检查构建产物
Write-Host ""
Write-Host "[3/4] 检查构建产物..."
$releaseDir = Join-Path $PSScriptRoot "release"
$setupExe = Get-ChildItem -Path $releaseDir -Filter "专注提醒器-*-win.exe" -ErrorAction SilentlyContinue | Select-Object -First 1
if (-not $setupExe) {
    Write-Error "未找到生成的 .exe 安装包，请检查 release/ 目录"
    exit 1
}

Write-Host "构建成功: $($setupExe.FullName)"
Write-Host "文件大小: $([math]::Round($setupExe.Length / 1MB, 2)) MB"

# 6. 输出后续步骤
Write-Host ""
Write-Host "[4/4] 后续步骤:"
Write-Host "  1. 在 release/ 目录找到 .exe 文件"
Write-Host "  2. 复制到 Windows 测试机进行安装测试"
Write-Host "  3. 验证: 安装、卸载、快捷方式、右下角定位、10秒测试模式"
Write-Host "  4. 如需正式发行，购买 Windows 代码签名证书并配置"
Write-Host ""
Write-Host "构建产物目录: $releaseDir"
