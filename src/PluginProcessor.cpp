#include "PluginProcessor.h"
#include "PluginEditor.h"

// ── Constructor / Destructor ─────────────────────────────────────────────────

HelloReaperProcessor::HelloReaperProcessor()
    : AudioProcessor (BusesProperties()
          .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "Parameters", createParameterLayout())
{
}

HelloReaperProcessor::~HelloReaperProcessor() {}

// ── Parameter layout ─────────────────────────────────────────────────────────

juce::AudioProcessorValueTreeState::ParameterLayout
HelloReaperProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // Gain: -60 dB … +12 dB, skewed so the dial feels natural
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "gain", "Gain",
        juce::NormalisableRange<float> (-60.0f, 12.0f, 0.1f, 2.5f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel ("dB")));

    layout.add (std::make_unique<juce::AudioParameterBool> ("mute", "Mute", false));

    return layout;
}

// ── Playback lifecycle ────────────────────────────────────────────────────────

void HelloReaperProcessor::prepareToPlay (double sampleRate, int /*samplesPerBlock*/)
{
    gainSmoothed.reset (sampleRate, 0.05); // 50 ms ramp — prevents clicks on parameter changes
    gainSmoothed.setCurrentAndTargetValue (
        juce::Decibels::decibelsToGain (apvts.getRawParameterValue ("gain")->load()));
}

void HelloReaperProcessor::releaseResources() {}

// ── Audio processing ─────────────────────────────────────────────────────────

void HelloReaperProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                          juce::MidiBuffer& /*midi*/)
{
    juce::ScopedNoDenormals noDenormals;

    const bool  muted      = apvts.getRawParameterValue ("mute")->load() > 0.5f;
    const float targetGain = muted
        ? 0.0f
        : juce::Decibels::decibelsToGain (apvts.getRawParameterValue ("gain")->load());

    gainSmoothed.setTargetValue (targetGain);

    // Apply smoothed gain sample-by-sample so ramps are artefact-free
    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        const float g = gainSmoothed.getNextValue();
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            buffer.getWritePointer (ch)[i] *= g;
    }

    // Update peak meters (atomic — safe to read from the editor's timer thread)
    if (buffer.getNumChannels() > 0)
        levelLeft.store  (buffer.getMagnitude (0, 0, buffer.getNumSamples()));
    if (buffer.getNumChannels() > 1)
        levelRight.store (buffer.getMagnitude (1, 0, buffer.getNumSamples()));
}

// ── Editor ───────────────────────────────────────────────────────────────────

juce::AudioProcessorEditor* HelloReaperProcessor::createEditor()
{
    return new HelloReaperEditor (*this);
}

// ── State persistence ─────────────────────────────────────────────────────────

void HelloReaperProcessor::getStateInformation (juce::MemoryBlock& dest)
{
    auto state = apvts.copyState();
    if (auto xml = state.createXml())
        copyXmlToBinary (*xml, dest);
}

void HelloReaperProcessor::setStateInformation (const void* data, int size)
{
    if (auto xml = getXmlFromBinary (data, size))
        if (xml->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

// ── Plugin entry point ────────────────────────────────────────────────────────

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new HelloReaperProcessor();
}
