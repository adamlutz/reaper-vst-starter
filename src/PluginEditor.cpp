#include "PluginEditor.h"

// ── Palette — mirrors the REAPER Stream web dashboard colours ────────────────
namespace Pal
{
    const juce::Colour bg     { 0xff07070a };
    const juce::Colour card   { 0xff111117 };
    const juce::Colour border { 0xff1e1e28 };
    const juce::Colour muted  { 0xff2a2a36 };
    const juce::Colour text   { 0xffc4c4d4 };
    const juce::Colour dim    { 0xff5a5a72 };
    const juce::Colour accent { 0xff7c6aff };
    const juce::Colour green  { 0xff22d97a };
    const juce::Colour amber  { 0xfff59e0b };
    const juce::Colour red    { 0xfff04060 };
}

// ── Custom LookAndFeel for the rotary dial ────────────────────────────────────
struct StudioLookAndFeel : public juce::LookAndFeel_V4
{
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                           juce::Slider&) override
    {
        const float radius  = (float) juce::jmin (width / 2, height / 2) - 6.0f;
        const float centreX = (float) x + (float) width  * 0.5f;
        const float centreY = (float) y + (float) height * 0.5f;
        const float angle   = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        // Track arc
        juce::Path track;
        track.addCentredArc (centreX, centreY, radius, radius, 0.0f,
                             rotaryStartAngle, rotaryEndAngle, true);
        g.setColour (Pal::muted);
        g.strokePath (track, juce::PathStrokeType (3.0f));

        // Fill arc
        juce::Path fill;
        fill.addCentredArc (centreX, centreY, radius, radius, 0.0f,
                            rotaryStartAngle, angle, true);
        g.setColour (Pal::accent);
        g.strokePath (fill, juce::PathStrokeType (3.0f,
            juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // Knob body
        g.setColour (Pal::card);
        g.fillEllipse (centreX - radius * 0.6f, centreY - radius * 0.6f,
                       radius * 1.2f, radius * 1.2f);
        g.setColour (Pal::border);
        g.drawEllipse (centreX - radius * 0.6f, centreY - radius * 0.6f,
                       radius * 1.2f, radius * 1.2f, 1.0f);

        // Indicator line
        const float lineLen = radius * 0.45f;
        g.setColour (Pal::accent);
        g.drawLine (centreX,
                    centreY,
                    centreX + std::sin (angle) * lineLen,
                    centreY - std::cos (angle) * lineLen,
                    2.0f);
    }
};

static StudioLookAndFeel studioLAF;

// ── Constructor ───────────────────────────────────────────────────────────────

HelloReaperEditor::HelloReaperEditor (HelloReaperProcessor& p)
    : AudioProcessorEditor (&p),
      processor (p),
      gainAttachment (p.apvts, "gain", gainSlider),
      muteAttachment (p.apvts, "mute", muteButton)
{
    setSize (320, 200);
    setResizable (true, true);
    setResizeLimits (260, 160, 640, 420);

    // ── Gain knob ──────────────────────────────────────────────────────────
    gainSlider.setLookAndFeel (&studioLAF);
    gainSlider.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    gainSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 72, 18);
    gainSlider.setColour (juce::Slider::textBoxTextColourId,       Pal::text);
    gainSlider.setColour (juce::Slider::textBoxBackgroundColourId, Pal::card);
    gainSlider.setColour (juce::Slider::textBoxOutlineColourId,    Pal::border);
    addAndMakeVisible (gainSlider);

    gainLabel.setText ("GAIN", juce::dontSendNotification);
    gainLabel.setJustificationType (juce::Justification::centred);
    gainLabel.setColour (juce::Label::textColourId, Pal::dim);
    gainLabel.setFont (juce::Font (10.0f).withExtraKerningFactor (0.15f));
    addAndMakeVisible (gainLabel);

    // ── Mute button ────────────────────────────────────────────────────────
    muteButton.setButtonText ("MUTE");
    muteButton.setClickingTogglesState (true);
    muteButton.setColour (juce::TextButton::buttonColourId,   Pal::muted);
    muteButton.setColour (juce::TextButton::buttonOnColourId, Pal::red.withAlpha (0.9f));
    muteButton.setColour (juce::TextButton::textColourOffId,  Pal::dim);
    muteButton.setColour (juce::TextButton::textColourOnId,   Pal::text);
    addAndMakeVisible (muteButton);

    startTimerHz (30); // 30 fps meter refresh
}

HelloReaperEditor::~HelloReaperEditor()
{
    stopTimer();
    gainSlider.setLookAndFeel (nullptr);
}

// ── Timer: smooth meter decay ─────────────────────────────────────────────────

void HelloReaperEditor::timerCallback()
{
    // Take the higher of the new peak or the decaying hold
    meterLeft  = std::max (processor.levelLeft.load(),  meterLeft  * 0.88f);
    meterRight = std::max (processor.levelRight.load(), meterRight * 0.88f);
    repaint();
}

// ── Paint ─────────────────────────────────────────────────────────────────────

void HelloReaperEditor::paint (juce::Graphics& g)
{
    g.fillAll (Pal::bg);

    // Title bar
    const auto titleBar = getLocalBounds().removeFromTop (28);
    g.setColour (Pal::card);
    g.fillRect (titleBar);
    g.setColour (Pal::border);
    g.drawLine (0.0f, 28.0f, (float) getWidth(), 28.0f, 1.0f);
    g.setColour (Pal::accent);
    g.setFont (juce::Font (10.0f).withExtraKerningFactor (0.2f));
    g.drawText ("HELLO REAPER  //  GAIN", titleBar, juce::Justification::centred);

    // ── VU meters ──────────────────────────────────────────────────────────
    const int mW   = 10;
    const int mGap = 3;
    const int mH   = getHeight() - 28 - 20;
    const int mTop = 28 + 10;
    const int mX   = getWidth() - 14 - (mW * 2 + mGap);

    auto drawMeter = [&] (float level, int x)
    {
        g.setColour (Pal::muted);
        g.fillRoundedRectangle ((float) x, (float) mTop, (float) mW, (float) mH, 2.0f);

        const float db   = juce::Decibels::gainToDecibels (level, -60.0f);
        const float norm = juce::jlimit (0.0f, 1.0f, (db + 60.0f) / 60.0f);
        const int   barH = juce::roundToInt (norm * (float) mH);

        const juce::Colour col = (db > -6.0f)  ? Pal::red
                               : (db > -18.0f) ? Pal::amber
                                               : Pal::green;
        g.setColour (col.withAlpha (0.85f));
        g.fillRoundedRectangle ((float) x,
                                (float) (mTop + mH - barH),
                                (float) mW,
                                (float) barH,
                                2.0f);
    };

    drawMeter (meterLeft,  mX);
    drawMeter (meterRight, mX + mW + mGap);

    // Meter labels
    g.setColour (Pal::dim);
    g.setFont (8.0f);
    g.drawText ("L   R",
                mX, mTop + mH + 3,
                mW * 2 + mGap + 2, 12,
                juce::Justification::centred);

    // Outer border
    g.setColour (Pal::border);
    g.drawRect (getLocalBounds(), 1);
}

// ── Resized ───────────────────────────────────────────────────────────────────

void HelloReaperEditor::resized()
{
    auto area = getLocalBounds();
    area.removeFromTop (32);           // title bar
    area.removeFromRight (44);         // meter zone

    auto ctrl = area.reduced (12, 6);
    gainLabel.setBounds (ctrl.removeFromTop (14));
    muteButton.setBounds (ctrl.removeFromBottom (26).reduced (16, 0));
    gainSlider.setBounds (ctrl);
}
