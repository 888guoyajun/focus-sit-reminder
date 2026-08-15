# 专注久坐提醒器 v0.1

这是一个第一版 AI 硬件原型，目标很简单：

- 专注满 45 分钟提醒一次
- 久坐超过阈值提醒一次
- 根据用户是否响应，自动调整提醒强度

## 第一版范围

- 主控：ESP32
- 传感：按钮模拟“用户响应”
- 反馈：LED、蜂鸣器、振动马达或舵机
- 连接：USB 供电

## 先不做

- 摄像头识别
- 云端账号系统
- 复杂 App
- 伤害性拍打结构

## 目录

- `firmware/`：ESP32 Arduino 固件
- `docs/`：元器件清单、接线说明、测试模板
- `hardware/`：后续放结构草图和 3D 打印文件
- `desktop/`：桌面右下角提醒器第一版（Electron）

## 启动桌面提醒器

```bash
cd desktop
npm install
npm start
```

桌面端默认开启开发测试模式，10 秒后触发提醒；在“设置”中取消后即使用真实的 45 分钟专注提醒。窗口固定在主屏幕右下角，数据只保存在本机浏览器存储中。

## 给用户安装

### macOS

```bash
cd desktop
npm install
npm run dist:mac
```

产物：`desktop/release/专注提醒器-{version}-mac.dmg`

### Windows（推荐：真实 Windows 环境）

在 Windows 10/11 上，以管理员权限打开 PowerShell：

```powershell
cd desktop
.\build-win.ps1
```

产物：`desktop/release/专注提醒器-{version}-win.exe`

### Windows（macOS 交叉构建，仅限测试）

> ⚠️ **当前状态**：2026-08-15 尝试 `brew install --cask wine-stable` 失败（下载源 502），macOS 交叉构建 Windows 安装包暂时不可行。

如果后续 Wine 下载恢复，可执行：

```bash
brew install --cask wine-stable
cd desktop
./build-win-mac.sh
```

产物：`desktop/release/专注提醒器-{version}-win.exe`

⚠️ macOS 交叉构建的 Windows 包仅为临时测试，正式发布前必须在真实 Windows 环境重新构建。

### GitHub Actions 自动构建（当前推荐）

已配置 `.github/workflows/build.yml`，推送 `v*` 标签后会自动构建 macOS DMG + Windows EXE 并发布 Release。

**前置条件**：本项目已初始化本地 Git 仓库，但尚未关联远程仓库。你需要：

1. 在 GitHub 创建空仓库（例如 `guoxiansheng/focus-sit-reminder`）
2. 关联并推送：

```bash
git remote add origin https://github.com/guoxiansheng/focus-sit-reminder.git
git branch -M main
git push -u origin main
```

3. 打标签触发自动构建：

```bash
git tag v0.1.1
git push origin v0.1.1
```

4. 在 GitHub Release 页面下载 `专注提醒器-0.1.1-win.exe`

构建产物会放在 `desktop/release/`。macOS 安装包需要在 macOS 上构建，Windows 安装包需要在 Windows 上构建；正式销售前还需要分别购买并配置 Apple Developer 签名/公证和 Windows 代码签名证书，否则用户可能看到系统安全警告。

### 签名与公证（正式发行前必须）

- **macOS**：Apple Developer 证书签名 + Notarization 公证
- **Windows**：EV 代码签名证书（消除 SmartScreen 警告）
- 测试阶段可用临时签名或自签名，但需告知用户如何放行
