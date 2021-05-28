#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
class AudioPluginAudioProcessorEditor  : public juce::AudioProcessorEditor
    , private juce::Timer
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    //==============================================================================
    virtual void timerCallback() override;

    AudioPluginAudioProcessor& processorRef;

    std::unique_ptr<juce::Slider> sliderGain;
    std::unique_ptr<juce::Slider> sliderFrequency;
    std::unique_ptr<juce::ComboBox> comboboxWaveform;
    std::unique_ptr<juce::Label> labelGain;
    std::unique_ptr<juce::Label> labelFrequency;
    std::unique_ptr<juce::Label> labelWaveform;
    std::unique_ptr<juce::GroupComponent> groupOscillator;

    std::unique_ptr<juce::Slider> sliderTremoloFrequnency;
    std::unique_ptr<juce::Slider> sliderTremoloDepth;
    std::unique_ptr<juce::ComboBox> comboboxTremoloWaveform;
    std::unique_ptr<juce::Label> labelTremoloFrequency;
    std::unique_ptr<juce::Label> labelTremoloDepth;
    std::unique_ptr<juce::Label> labelTremoloWaveform;
    std::unique_ptr<juce::GroupComponent> groupTremolo;

    juce::OwnedArray<juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttachments;
    juce::OwnedArray<juce::AudioProcessorValueTreeState::ComboBoxAttachment> comboboxAttachments;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
