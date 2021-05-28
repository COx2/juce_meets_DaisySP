#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
    , groupOscillator(std::make_unique<juce::GroupComponent>("Oscillator", "Oscillator"))
    , sliderGain(std::make_unique<juce::Slider>(juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow))
    , sliderFrequency(std::make_unique<juce::Slider>(juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow))
    , comboboxWaveform(std::make_unique<juce::ComboBox>())
    , labelGain(std::make_unique<juce::Label>("Gain", "GAIN"))
    , labelFrequency(std::make_unique<juce::Label>("Frequency", "FREQ"))
    , labelWaveform(std::make_unique<juce::Label>("Waveform", "WAVE"))
    , groupTremolo(std::make_unique<juce::GroupComponent>("Tremolo", "Tremolo"))
    , sliderTremoloFrequnency(std::make_unique<juce::Slider>(juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow))
    , sliderTremoloDepth(std::make_unique<juce::Slider>(juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow))
    , comboboxTremoloWaveform(std::make_unique<juce::ComboBox>())
    , labelTremoloFrequency(std::make_unique<juce::Label>("Frequency", "FREQ"))
    , labelTremoloDepth(std::make_unique<juce::Label>("Depth", "DEPTH"))
    , labelTremoloWaveform(std::make_unique<juce::Label>("Waveform", "WAVE"))
{
    addAndMakeVisible(groupOscillator.get());

    labelGain->attachToComponent(sliderGain.get(), false);
    labelGain->setJustificationType(juce::Justification::centred);

    sliderAttachments.add(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
        (processorRef.getProcessorState(), "Gain", *sliderGain));
    addAndMakeVisible(sliderGain.get());

    labelFrequency->attachToComponent(sliderFrequency.get(), false);
    labelFrequency->setJustificationType(juce::Justification::centred);

    sliderAttachments.add(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
        (processorRef.getProcessorState(), "Frequency", *sliderFrequency));
    addAndMakeVisible(sliderFrequency.get());

    labelWaveform->attachToComponent(comboboxWaveform.get(), false);
    labelWaveform->setJustificationType(juce::Justification::centred);

    comboboxAttachments.add(std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>
        (processorRef.getProcessorState(), "Waveform", *comboboxWaveform));
    comboboxWaveform->setJustificationType(juce::Justification::centred);
    comboboxWaveform->addItemList(processorRef.getProcessorState().getParameter("Waveform")->getAllValueStrings(), 1);
    comboboxWaveform->setText(processorRef.getProcessorState().getParameter("Waveform")->getCurrentValueAsText());
    addAndMakeVisible(comboboxWaveform.get());

    addAndMakeVisible(groupTremolo.get());

    labelTremoloFrequency->attachToComponent(sliderTremoloFrequnency.get(), false);
    labelTremoloFrequency->setJustificationType(juce::Justification::centred);

    sliderAttachments.add(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
        (processorRef.getProcessorState(), "TremoloFrequency", *sliderTremoloFrequnency));
    addAndMakeVisible(sliderTremoloFrequnency.get());

    labelTremoloDepth->attachToComponent(sliderTremoloDepth.get(), false);
    labelTremoloDepth->setJustificationType(juce::Justification::centred);

    sliderAttachments.add(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
        (processorRef.getProcessorState(), "TremoloDepth", *sliderTremoloDepth));
    addAndMakeVisible(sliderTremoloDepth.get());

    labelTremoloWaveform->attachToComponent(comboboxTremoloWaveform.get(), false);
    labelTremoloWaveform->setJustificationType(juce::Justification::centred);

    comboboxAttachments.add(std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>
        (processorRef.getProcessorState(), "TremoloWaveform", *comboboxTremoloWaveform));
    comboboxTremoloWaveform->setJustificationType(juce::Justification::centred);
    comboboxTremoloWaveform->addItemList(processorRef.getProcessorState().getParameter("TremoloWaveform")->getAllValueStrings(), 1);
    comboboxTremoloWaveform->setText(processorRef.getProcessorState().getParameter("TremoloWaveform")->getCurrentValueAsText());
    addAndMakeVisible(comboboxTremoloWaveform.get());


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
    const juce::Rectangle<float> drawArea = { bounds.getWidth() * 0.1f, bounds.getHeight() * 0.75f, bounds.getWidth() * 0.8f, bounds.getHeight() * 0.225f };
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

    groupOscillator->setBounds(juce::Rectangle<int>(bounds.getWidth() * 0.05f, bounds.getHeight() * 0.015f, bounds.getWidth() * 0.9f, bounds.getHeight() * 0.35f));
    
    groupTremolo->setBounds(juce::Rectangle<int>(bounds.getWidth() * 0.05f, bounds.getHeight() * 0.38f, bounds.getWidth() * 0.9f, bounds.getHeight() * 0.35f));

    const int slider_w = bounds.getWidth() * 0.2f;
    const int slider_h = bounds.getHeight() * 0.2f;
    const int combobox_w = bounds.getWidth() * 0.2f;
    const int combobox_h = bounds.getHeight() * 0.1f;

    {
        sliderGain->setSize(slider_w, slider_h);
        sliderGain->setCentrePosition(
            groupOscillator->getX() + groupOscillator->getWidth() * 0.2f, 
            groupOscillator->getY() + groupOscillator->getHeight() * 0.6f);

        comboboxWaveform->setSize(combobox_w, combobox_h);
        comboboxWaveform->setCentrePosition(
            groupOscillator->getX() + groupOscillator->getWidth() * 0.5f,
            groupOscillator->getY() + groupOscillator->getHeight() * 0.5f);

        sliderFrequency->setSize(slider_w, slider_h);
        sliderFrequency->setCentrePosition(
            groupOscillator->getX() + groupOscillator->getWidth() * 0.8f, 
            groupOscillator->getY() + groupOscillator->getHeight() * 0.6f);
    }

    {
        sliderTremoloFrequnency->setSize(slider_w, slider_h);
        sliderTremoloFrequnency->setCentrePosition(
            groupTremolo->getX() + groupTremolo->getWidth() * 0.2f,
            groupTremolo->getY() + groupTremolo->getHeight() * 0.6f);

        comboboxTremoloWaveform->setSize(combobox_w, combobox_h);
        comboboxTremoloWaveform->setCentrePosition(
            groupTremolo->getX() + groupTremolo->getWidth() * 0.5f,
            groupTremolo->getY() + groupTremolo->getHeight() * 0.5f);

        sliderTremoloDepth->setSize(slider_w, slider_h);
        sliderTremoloDepth->setCentrePosition(
            groupTremolo->getX() + groupTremolo->getWidth() * 0.8f,
            groupTremolo->getY() + groupTremolo->getHeight() * 0.6f);
    }
}

void AudioPluginAudioProcessorEditor::timerCallback()
{
    repaint();
}
