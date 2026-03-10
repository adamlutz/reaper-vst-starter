#pragma once
#include <JuceHeader.h>

/**
 * HelloReaperProcessor
 * ────────────────────
 * A minimal stereo gain plugin — the VST "Hello World".
 *
 * Parameters:
 *   gain  – output level in dB  (-60 … +12)
 *   mute  – hard mute           (bool)
 *
 * Exposes atomic level meters (levelLeft / levelRight) so the
 * editor can display a peak VU without locking the audio thread.
 */
class HelloReaperProcessor : public juce::AudioProcessor
{
public:
    HelloReaperProcessor();
    ~HelloReaperProcessor() override;

    // ── AudioProcessor interface ───────────────────────────────────────────
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }

    bool  acceptsMidi()  const override { return false; }
    bool  producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int  getNumPrograms()   override { return 1; }
    int  getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& dest) override;
    void setStateInformation (const void* data, int size) override;

    // ── Public state (read by editor on timer thread) ──────────────────────
    juce::AudioProcessorValueTreeState apvts;
    std::atomic<float> levelLeft  { 0.0f };
    std::atomic<float> levelRight { 0.0f };

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    juce::LinearSmoothedValue<float> gainSmoothed;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HelloReaperProcessor)
};
