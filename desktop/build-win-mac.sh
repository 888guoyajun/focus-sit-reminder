#!/usr/bin/env bash
# 专注久坐提醒器 - macOS 交叉构建 Windows 安装包脚本
# 需要提前安装 Wine: brew install --cask wine-stable
# 说明：Mac 上交叉构建 Windows NSIS 安装包可能不稳定，优先在真实 Windows 环境构建。

set -e

echo "========================================="
echo "专注久坐提醒器 macOS 交叉构建 Windows 脚本"
echo "========================================="

# 检查 Node.js
if ! command -v node &> /dev/null; then
    echo "错误：未检测到 Node.js"
    exit 1
fi
echo "Node.js 版本: $(node --version)"

# 检查 Wine
if ! command -v wine64 &> /dev/null && ! command -v wine &> /dev/null; then
    echo "警告：未检测到 Wine，macOS 交叉构建 Windows 包需要 Wine"
    echo "安装方式: brew install --cask wine-stable"
    echo ""
    read -p "是否继续尝试构建？(y/N) " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi

# 安装依赖
echo ""
echo "[1/3] 安装 npm 依赖..."
npm install

# 交叉构建 Windows 包
echo ""
echo "[2/3] 交叉构建 Windows NSIS 安装包..."
# electron-builder 在 Mac 上构建 Windows 需要 Wine
npm run dist:win

# 检查产物
echo ""
echo "[3/3] 检查构建产物..."
RELEASE_DIR="$(cd "$(dirname "$0")" && pwd)/release"
SETUP_EXE=$(find "$RELEASE_DIR" -maxdepth 1 -name "专注提醒器-*-win.exe" -print -quit)

if [ -z "$SETUP_EXE" ]; then
    echo "错误：未找到生成的 .exe 安装包"
    echo "可能原因：Wine 未正确安装或 electron-builder 配置问题"
    exit 1
fi

echo "构建成功: $SETUP_EXE"
echo "文件大小: $(du -h "$SETUP_EXE" | cut -f1)"

echo ""
echo "重要提示："
echo "  1. macOS 交叉构建的 Windows 包仅为临时测试使用"
echo "  2. 正式发布前必须在真实 Windows 环境重新构建并测试"
echo "  3. 正式包需要 Windows 代码签名证书，否则用户会看到 SmartScreen 警告"
echo ""
echo "构建产物目录: $RELEASE_DIR"
