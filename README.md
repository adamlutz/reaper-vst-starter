```
                                     ___
                                 _,-'   '-._
                              ,-'   .-"-.   '-.
                             /    .` o   o `.    \
                            /   .'    .-.    `.   \
     _____________________./   /    (_____)    \   \.____________________
    /                      |   |  .-'     '-.  |   |                     \
   /    ,---.               \   \ /   ,-.    \ /   /               ,---.  \
  /   .'     |               '._ '-._____.-' _.' _.'               |     '.  \
 /   /    ,--'                  '-.__________.-'                  '--,    \   \
|   |   .'                           | |                           '.   |   |
|   |  /                            _| |_                            \  |   |
|   | |                            |     |                            | |   |
 \   \ |                           |     |                           | /   /
  \   ''--.__                      |_____|                      __,--''   /
   \        ''--.___________  _____|     |_____  ___________.--''        /
    \___________________________/           \___________________________/

        ██╗  ██╗███████╗██╗     ██╗      ██████╗     ██████╗ ███████╗ █████╗ ██████╗ ███████╗██████╗
        ██║  ██║██╔════╝██║     ██║     ██╔═══██╗    ██╔══██╗██╔════╝██╔══██╗██╔══██╗██╔════╝██╔══██╗
        ███████║█████╗  ██║     ██║     ██║   ██║    ██████╔╝█████╗  ███████║██████╔╝█████╗  ██████╔╝
        ██╔══██║██╔══╝  ██║     ██║     ██║   ██║    ██╔══██╗██╔══╝  ██╔══██║██╔═══╝ ██╔══╝  ██╔══██╗
        ██║  ██║███████╗███████╗███████╗╚██████╔╝    ██║  ██║███████╗██║  ██║██║     ███████╗██║  ██║
        ╚═╝  ╚═╝╚══════╝╚══════╝╚══════╝ ╚═════╝     ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝╚═╝     ╚══════╝╚═╝  ╚═╝

                              V S T 3   ·   A U   ·   S T A N D A L O N E
```

# reaper-vst-starter

A hello-world VST3/AU gain plugin for REAPER built with [JUCE](https://juce.com/).
Stereo gain knob, mute button, L/R peak VU meters, dark studio UI.
Companion to [reaper-stream](https://github.com/adamlutz/reaper-stream).

---

## Quick start (macOS)

### 1. Install dependencies

```bash
xcode-select --install          # Xcode command line tools
brew install cmake ninja        # build tools
```

> Xcode itself is required for the AU target — install it from the App Store if you haven't already.
> **JUCE is downloaded automatically** during the CMake configure step, no manual install needed.

### 2. Clone and build

```bash
git clone git@github.com:adamlutz/reaper-vst-starter.git
cd reaper-vst-starter

cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

The first configure takes ~30 s while CMake fetches JUCE. The build itself takes 1–2 minutes.

### 3. Install the plugin

```bash
cp -r "build/HelloReaper_artefacts/Release/VST3/Hello REAPER.vst3" \
      ~/Library/Audio/Plug-Ins/VST3/
```

### 4. Load it in REAPER

1. **Options → Preferences → Plug-ins/VST**
2. Confirm `~/Library/Audio/Plug-Ins/VST3` is in the scan paths — add it if not
3. Click **Clear cache and re-scan**
4. Open any track's FX chain and search for **Hello REAPER**

---

## What's in the box

| File | Purpose |
|------|---------|
| `CMakeLists.txt` | Build config — fetches JUCE, declares VST3/AU/Standalone targets |
| `src/PluginProcessor.cpp` | DSP: smoothed gain, mute, peak metering |
| `src/PluginEditor.cpp` | UI: rotary dial, VU bars, dark theme matching reaper-stream |

---

## Related

- [reaper-stream](https://github.com/adamlutz/reaper-stream) — live audio + MIDI web dashboard for REAPER
- [JUCE docs](https://docs.juce.com)
