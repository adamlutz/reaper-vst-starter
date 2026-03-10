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

A hello-world VST3/AU gain plugin for REAPER, built with [JUCE](https://juce.com/).
Companion project to [reaper-stream](https://github.com/adamlutz/reaper-stream).

**What it does:**
- Stereo gain control (-60 dB → +12 dB) with a smooth 50 ms ramp
- Hard mute button
- L/R peak VU meters in the plugin window
- Dark studio UI — same colour palette as the REAPER Stream dashboard

---

## Prerequisites

| Tool | Version | Notes |
|------|---------|-------|
| **CMake** | 3.22+ | `brew install cmake` |
| **Xcode** | 14+ | macOS / AU target — install from App Store |
| **Xcode Command Line Tools** | latest | `xcode-select --install` |
| **Ninja** *(optional)* | any | `brew install ninja` — faster builds |

> JUCE is fetched automatically by CMake via `FetchContent` — no manual download needed.

---

## Build

```bash
# Clone
git clone git@github.com:adamlutz/reaper-vst-starter.git
cd reaper-vst-starter

# Configure  (Xcode generator)
cmake -B build -G Xcode

# Build VST3 + AU + Standalone
cmake --build build --config Release

# Or with Ninja (faster)
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Built artefacts land in:
```
build/HelloReaper_artefacts/Release/
├── VST3/   Hello REAPER.vst3
├── AU/     Hello REAPER.component
└── Standalone/   Hello REAPER.app
```

---

## Install into REAPER

### VST3 (recommended)

```bash
cp -r "build/HelloReaper_artefacts/Release/VST3/Hello REAPER.vst3" \
      ~/Library/Audio/Plug-Ins/VST3/
```

### AU (Audio Units)

```bash
cp -r "build/HelloReaper_artefacts/Release/AU/Hello REAPER.component" \
      ~/Library/Audio/Plug-Ins/Components/
```

Then in REAPER:

1. **Options → Preferences → Plug-ins/VST**
2. Click **Re-scan** (or **Clear cache and re-scan**)
3. Add the plugin to any track via the FX chain

---

## Project structure

```
reaper-vst-hello/
├── CMakeLists.txt          # Build config — JUCE fetched here
├── src/
│   ├── PluginProcessor.h   # AudioProcessor declaration
│   ├── PluginProcessor.cpp # Gain + mute DSP, state persistence
│   ├── PluginEditor.h      # GUI declaration
│   └── PluginEditor.cpp    # Rotary dial, VU meters, dark theme
└── README.md
```

---

## Architecture

```
                REAPER FX chain
                      │
          ┌───────────▼───────────┐
          │   HelloReaperProcessor │
          │                       │
          │  AudioBuffer<float>   │
          │  ──────────────────►  │  gain (smoothed)
          │                       │  mute (hard)
          │  levelLeft / Right ──►│──── atomic floats
          └───────────────────────┘
                      │ createEditor()
          ┌───────────▼───────────┐
          │   HelloReaperEditor   │
          │                       │
          │  [Gain knob]  [MUTE]  │  ← APVTS attachments
          │  L ██  R ██           │  ← 30 Hz timer / repaint
          └───────────────────────┘
```

**Key JUCE concepts demonstrated:**

| Concept | Where |
|---------|-------|
| `AudioProcessorValueTreeState` | Parameter management + state save/load |
| `LinearSmoothedValue` | Click-free gain ramping |
| `FetchContent` (CMake) | Self-contained build — no submodule needed |
| `SliderAttachment` / `ButtonAttachment` | Binding UI widgets to parameters |
| `LookAndFeel_V4` | Custom rotary dial rendering |
| Atomic level metering | Thread-safe audio → UI communication |

---

## Extending this plugin

| Idea | How |
|------|-----|
| Send level data to **reaper-stream** | UDP socket → `server.js` port 9001 — matches the Lua bridge format |
| Add a peak hold indicator | Store peak + timestamp in the editor, draw a tick mark |
| Multi-band EQ | Add per-band `AudioParameterFloat` + `IIRFilter` per channel |
| MIDI learn | `AudioProcessorValueTreeState::addParameterListener` |

---

## Related

- [reaper-stream](https://github.com/adamlutz/reaper-stream) — WebSocket audio + MIDI dashboard for REAPER
- [JUCE docs](https://docs.juce.com)
- [REAPER SDK / ReaScript docs](https://www.reaper.fm/sdk/reascript/reascripthelp.html)
