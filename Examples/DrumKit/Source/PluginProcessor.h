#pragma once

#include <JuceHeader.h>
#include "WaveShapeVisualizer.h"
#include "Drums/synthbassdrum.h"
#include "Drums/synthsnaredrum.h"
#include "Drums/hihat.h"

//==============================================================================
class AudioPluginAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    AudioPluginAudioProcessor();
    ~AudioPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    juce::AudioProcessorValueTreeState& getProcessorState() { return apvts; }
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout() const;

    WaveDrawBuffer& getWaveDrawBuffer() { return waveDrawBuffer; }

    void triggerBassDrum();
    void triggerSnareDrum();
    void triggerHiHat();

private:
    //==============================================================================
    juce::AudioProcessorValueTreeState apvts;

    std::unique_ptr<daisysp::SyntheticBassDrum> bassdrum;
    std::unique_ptr<daisysp::SyntheticSnareDrum> snaredrum;
    std::unique_ptr<daisysp::HiHat<>> hihat;

    // Wave shape visualizer
    WaveDrawBuffer waveDrawBuffer;
    WaveSampleCollector waveSampleCollector;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};
