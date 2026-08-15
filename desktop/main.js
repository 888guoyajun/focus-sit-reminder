const { app, BrowserWindow, screen, ipcMain } = require('electron');
const path = require('node:path');

let mainWindow;

function positionWindow() {
  if (!mainWindow || mainWindow.isDestroyed()) return;
  const display = screen.getPrimaryDisplay();
  const { width, height } = display.workAreaSize;
  const [windowWidth, windowHeight] = mainWindow.getSize();
  mainWindow.setPosition(width - windowWidth - 22, height - windowHeight - 22, false);
}

function createWindow() {
  mainWindow = new BrowserWindow({
    width: 360,
    height: 430,
    minWidth: 340,
    minHeight: 400,
    maxWidth: 420,
    maxHeight: 520,
    frame: false,
    resizable: false,
    alwaysOnTop: true,
    skipTaskbar: false,
    show: false,
    backgroundColor: '#10161d',
    webPreferences: {
      preload: path.join(__dirname, 'preload.js'),
      contextIsolation: true,
      sandbox: true
    }
  });

  mainWindow.loadFile(path.join(__dirname, 'index.html'));
  mainWindow.once('ready-to-show', () => {
    positionWindow();
    mainWindow.show();
  });
  mainWindow.on('closed', () => { mainWindow = null; });
}

app.whenReady().then(() => {
  createWindow();
  screen.on('display-metrics-changed', positionWindow);
  screen.on('display-added', positionWindow);
  screen.on('display-removed', positionWindow);
});

ipcMain.on('window-minimize', () => mainWindow?.minimize());
ipcMain.on('window-close', () => app.quit());

app.on('window-all-closed', () => app.quit());
