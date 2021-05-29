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

    juce::Rectangle<int> rectDrumKit;
    std::unique_ptr<juce::TextButton> buttonBassDrum;
    std::unique_ptr<juce::TextButton> buttonSnareDrum;
    std::unique_ptr<juce::TextButton> buttonHiHat;

    std::unique_ptr<juce::MidiKeyboardComponent> midiKeyboard;

    juce::OwnedArray<juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttachments;
    juce::OwnedArray<juce::AudioProcessorValueTreeState::ComboBoxAttachment> comboboxAttachments;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
