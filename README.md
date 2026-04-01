# GIF Overlay
A small desktop app that lets you display your favourite .gif floating on top of all your windows. 
Enabled moving, resizing, mirroring, and hiding the displayed window whenever you want.
Build with C++ and Qt6, and with the help of Claude, who helped me through getting familiar with Qt6's features and development framework.

## Features
- Always on top of every window, including your browser.
- Drag the window anywhere on your screen (except the bottom tool bars).
- Resize the window from the bottom-right corner handle.
- Mirror the .gif horizontally with top-left corner button.
- Hide the window with the top-right cross button; bring it back from the system tray.
- Transparent background.
- Remembers .gif file's position, size, and settings between launches.
- Load any .gif file from the tray menu.

## Environment Requirement
- Windows 10 or 11
- Qt 6.x (with MinGW 64-bit)
- Qt Creator

## How to Build
1. Clone or download this repository.
2. Open Qt Creator.
3. In Qt Creator, click "File -> Open File or Project", and select 'GifOverlay.pro'.
4. Select "Desktop Qt6.x.x MinGW 64-bit" as your kit.
5. Click "Configure Project".
6. Press "Ctrl + R" to build and run.

## How to Use

| Action | How |
|---|---|
| Move the GIF | Click and drag it |
| Resize | Drag the handle at the bottom-right corner |
| Mirror | Click the arrow button at the top-left |
| Hide | Click the cross button at the top-right |
| Show again | Click the tray icon, or right-click → Show GIF |
| Load a GIF | Right-click tray icon → Load GIF file... |
| Quit | Right-click tray icon → Quit |

## How to Make a Standalone.exe
1. Open Qt Creator and open the project.
2. Switch to "Release" mode at the bottom-left monitor icon.
3. Press "Ctrl + B" to build.
4. Open "Qt MinGW terminal" from Start menu.
5. Swich to your release folder in the terminal:
```
   E:
   cd "path\to\your\release\folder"
```
6. Run the .exe in the terminal:
```
   windeployqt GifOverlay.exe
```
7. The folder in release is now self-contained, so you can copy it anywhere or put a shortcut on the desktop.

## Known Issues
- The grey border around the displayed window is still being worked on.

## Future Ideas
- More fun buttons - make it a more implemented desktop pet.
