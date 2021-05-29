#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
    , buttonBassDrum(std::make_unique<juce::TextButton>("BassDrum", "BassDrum"))
    , buttonSnareDrum(std::make_unique<juce::TextButton>("SnareDrum", "SnareDrum"))
    , buttonHiHat(std::make_unique<juce::TextButton>("HiHat", "HiHat"))
    , midiKeyboard(std::make_unique<juce::MidiKeyboardComponent>(processorRef.getMidiKeyboardState(), juce::MidiKeyboardComponent::Orientation::horizontalKeyboard))
{
    addAndMakeVisible(midiKeyboard.get());

    buttonBassDrum->onClick = [this]() {
        processorRef.triggerBassDrum();
    };
    addAndMakeVisible(buttonBassDrum.get());

    buttonSnareDrum->onClick = [this]() {
        processorRef.triggerSnareDrum();
    };
    addAndMakeVisible(buttonSnareDrum.get());

    buttonHiHat->onClick = [this]() {
        processorRef.triggerHiHat();
    };
    addAndMakeVisible(buttonHiHat.get());

    setSize(600, 400);

    startTimerHz(30);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    sliderAttachments.clear();
    comboboxAttachments.clear();
}

//==============================================================================
void drawWaveShape(juce::Graphics& g, const juce::Rectangle<float>& drawArea, const float* plotData, const int numSamples)
{
    juce::Path wavePath;
    const float x0 = drawArea.getX();
    const float cloped_sample0 = juce::jmax<float>(-1.0f, juce::jmin<float>(1.0f, plotData[0]));
    const float y0 = juce::jmap<float>(cloped_sample0, -1.0f, 1.0f, drawArea.getBottom(), drawArea.getY());
    wavePath.startNewSubPath(x0, y0);

    for (int i = 1; i < numSamples; ++i)
    {
        const float x = juce::jmap<float>(i, 0, numSamples, x0, x0 + drawArea.getWidth());
        const float cloped_sample = juce::jmax<float>(-1.0f, juce::jmin<float>(1.0f, plotData[i]));
        const float y = juce::jmap<float>(cloped_sample, -1.0f, 1.0f, drawArea.getBottom(), drawArea.getY());
        wavePath.lineTo(x, y);
    }
    g.setColour(juce::Colours::cyan);
    g.strokePath(wavePath, juce::PathStrokeType(2.0f));
}

void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    const auto bounds = getLocalBounds();

    // Draw wave shape background
    const juce::Rectangle<float> drawArea = { bounds.getWidth() * 0.1f, bounds.getHeight() * 0.5f, bounds.getWidth() * 0.8f, bounds.getHeight() * 0.225f };
    g.setColour(juce::Colours::darkgrey);
    g.fillRect(drawArea);

    // Draw wave shape
    juce::AudioBuffer<float> samplesToDraw(1, processorRef.getWaveDrawBuffer().getBufferSize());
    processorRef.getWaveDrawBuffer().pop(samplesToDraw.getWritePointer(0), samplesToDraw.getNumSamples());
    drawWaveShape(g, drawArea, samplesToDraw.getWritePointer(0), samplesToDraw.getNumSamples());
}

void AudioPluginAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    rectDrumKit = juce::Rectangle<int>(bounds.getWidth() * 0.05f, bounds.getHeight() * 0.015f, bounds.getWidth() * 0.9f, bounds.getHeight() * 0.35f);

    const int button_w = bounds.getWidth() * 0.2f;
    const int button_h = bounds.getHeight() * 0.2f;

    {
        buttonBassDrum->setSize(button_w, button_h);
        buttonBassDrum->setCentrePosition(
            rectDrumKit.getX() + rectDrumKit.getWidth() * 0.2f,
            rectDrumKit.getY() + rectDrumKit.getHeight() * 0.6f);

        buttonSnareDrum->setSize(button_w, button_h);
        buttonSnareDrum->setCentrePosition(
            rectDrumKit.getX() + rectDrumKit.getWidth() * 0.5f,
            rectDrumKit.getY() + rectDrumKit.getHeight() * 0.6f);

        buttonHiHat->setSize(button_w, button_h);
        buttonHiHat->setCentrePosition(
            rectDrumKit.getX() + rectDrumKit.getWidth() * 0.8f,
            rectDrumKit.getY() + rectDrumKit.getHeight() * 0.6f);
    }

    const auto rect_keyboard = juce::Rectangle<int>(bounds.getWidth() * 0.0f, bounds.getHeight() * 0.8f, bounds.getWidth() * 1.0f, bounds.getHeight() * 0.2f);
    midiKeyboard->setBounds(rect_keyboard);
}

void AudioPluginAudioProcessorEditor::timerCallback()
{
    repaint();
}
