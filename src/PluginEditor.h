#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

/**
 * HelloReaperEditor
 * ─────────────────
 * Dark studio UI — matches the REAPER Stream web dashboard palette.
 *
 * Layout:
 *   ┌─────────────────────────────────┐
 *   │  HELLO REAPER  //  GAIN         │  ← title bar
 *   ├──────────────────────┬──────────┤
 *   │                      │ L  R     │
 *   │   [GAIN knob]        │ ██ ██    │  ← VU meters
 *   │                      │ ██ ██    │
 *   │   [ MUTE ]           │         │
 *   └──────────────────────┴──────────┘
 */
class HelloReaperEditor : public juce::AudioProcessorEditor,
                          private juce::Timer
{
public:
    explicit HelloReaperEditor (HelloReaperProcessor&);
    ~HelloReaperEditor() override;

    void paint   (juce::Graphics&) override;
    void resized () override;

private:
    void timerCallback() override;

    HelloReaperProcessor& processor;

    juce::Slider       gainSlider;
    juce::Label        gainLabel;
    juce::TextButton   muteButton;

    juce::AudioProcessorValueTreeState::SliderAttachment gainAttachment;
    juce::AudioProcessorValueTreeState::ButtonAttachment muteAttachment;

    // Smoothed meter values (decayed in the timer callback)
    float meterLeft  { 0.0f };
    float meterRight { 0.0f };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HelloReaperEditor)
};
