cmake_minimum_required(VERSION 3.22)
project(HelloReaper VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Fetch JUCE ──────────────────────────────────────────────────────────────
include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        7.0.12
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(JUCE)

# ── Plugin target ───────────────────────────────────────────────────────────
juce_add_plugin(HelloReaper
    COMPANY_NAME                "REAPER Stream"
    COMPANY_WEBSITE             "https://github.com/adamlutz/reaper-stream"
    PLUGIN_MANUFACTURER_CODE    Rst1
    PLUGIN_CODE                 Hlrp
    FORMATS                     VST3 AU Standalone
    PRODUCT_NAME                "Hello REAPER"
    PLUGIN_DESCRIPTION          "Hello-world VST3 gain plugin for REAPER"
    IS_SYNTH                    FALSE
    NEEDS_MIDI_INPUT            FALSE
    NEEDS_MIDI_OUTPUT           FALSE
    IS_MIDI_EFFECT              FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    VST3_CATEGORIES             "Fx" "Dynamics"
    AU_MAIN_TYPE                kAudioUnitType_Effect
    COPY_PLUGIN_AFTER_BUILD     FALSE
)

# ── Generate JuceHeader.h ────────────────────────────────────────────────────
juce_generate_juce_header(HelloReaper)

# ── Sources ─────────────────────────────────────────────────────────────────
target_sources(HelloReaper PRIVATE
    src/PluginProcessor.cpp
    src/PluginEditor.cpp
)

# ── Compile definitions ─────────────────────────────────────────────────────
target_compile_definitions(HelloReaper PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
)

# ── Link libraries ──────────────────────────────────────────────────────────
target_link_libraries(HelloReaper
    PRIVATE
        juce::juce_audio_utils
        juce::juce_gui_basics
    PUBLIC
        juce::juce_recommended_config_flags
)
