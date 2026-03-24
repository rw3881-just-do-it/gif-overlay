# GIF Overlay — Qt6 Desktop App for Windows

A frameless, always-on-top desktop app that displays an animated GIF floating
over all your windows. Drag it anywhere, hide it to the system tray, and it
remembers its position between sessions.

---

## Features

- Always on top of every window (including your browser)
- Drag to reposition anywhere on screen
- Hide/show via the ✕ button or system tray icon
- Click the tray icon to toggle visibility
- Remembers position and GIF path between launches
- Load any .gif file at runtime via the tray menu

---

## Step 1 — Install Qt6

1. Go to: https://www.qt.io/download-qt-installer
2. Download the **Qt Online Installer** (free, open source option)
3. Run the installer and sign in (free Qt account required)
4. Under **Qt 6.x.x**, check:
   - `MSVC 2022 64-bit`  ← if you have Visual Studio installed
   - OR `MinGW 64-bit`   ← simpler, no Visual Studio needed
5. Also check **Qt Creator** under the Tools section
6. Complete the install (it's large — ~5GB, takes a while)

---

## Step 2 — Install a Compiler

**Option A — MinGW (easier, no extra install)**
- Select `MinGW 64-bit` in the Qt installer above. That's it.

**Option B — MSVC (if you already have Visual Studio)**
- Install Visual Studio 2022 Community (free):
  https://visualstudio.microsoft.com/
- During install, select: **"Desktop development with C++"**

---

## Step 3 — Open the Project in Qt Creator

1. Open **Qt Creator** (installed with Qt)
2. Click **File → Open File or Project...**
3. Navigate to this folder and select **GifOverlay.pro**
4. Qt Creator will ask you to configure the project — select your
   installed kit (MinGW or MSVC) and click **Configure Project**

---

## Step 4 — Build & Run

1. Press **Ctrl+R** (or click the green ▶ Run button)
2. Qt Creator will compile and launch the app
3. You'll see a small placeholder window and a tray icon appear

---

## Step 5 — Load Your GIF

1. Right-click the **system tray icon** (bottom-right of your taskbar)
2. Click **"Load GIF file..."**
3. Browse to your `.gif` file and select it
4. The GIF will appear floating on your screen!

---

## How to Use

| Action                  | How                                              |
|-------------------------|--------------------------------------------------|
| Move the GIF            | Click and drag it                                |
| Hide the GIF            | Click the ✕ button, or click the tray icon       |
| Show the GIF again      | Click the tray icon, or right-click → Show GIF   |
| Load a different GIF    | Right-click tray icon → Load GIF file...         |
| Quit the app            | Right-click tray icon → Quit                     |

Position and GIF path are saved automatically and restored on next launch.

---

## Project File Structure

```
gif-overlay-qt/
├── GifOverlay.pro     ← Qt project file (open this in Qt Creator)
├── main.cpp           ← Entry point
├── GifOverlay.h       ← Widget class declaration
├── GifOverlay.cpp     ← Widget class implementation
├── resources.qrc      ← Qt resource file
└── README.md          ← This file
```

---

## Customization

**Change the default GIF size cap (currently 300px wide):**
In `GifOverlay.cpp`, find `qMin(gifSize.width(), 300)` and change 300
to whatever pixel width you prefer.

**Make it launch on Windows startup:**
Add a registry entry pointing to your built `.exe`:
Key: `HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run`
Value: `GifOverlay` = `"C:\path\to\GifOverlay.exe"`

**Build a release .exe (standalone):**
1. In Qt Creator, switch from **Debug** to **Release** mode
   (click the monitor icon bottom-left → Release)
2. Build with Ctrl+B
3. Run `windeployqt GifOverlay.exe` in the build folder to bundle
   all required Qt DLLs alongside your .exe
