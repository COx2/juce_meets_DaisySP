#pragma once

#include <JuceHeader.h>
#include "WaveShapeVisualizer.h"
#include "Synthesis/oscillator.h"
#include "Effects/tremolo.h"

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

private:
    //==============================================================================
    enum WaveformType
    {
        kUnknown = -1,
        kSine = 0,
        kSquare,
        kTriangle,
        kSaw,
    };
    const juce::StringArray waveformTypes{ "Sine", "Square", "Triangle", "Saw"};
    
    //==============================================================================
    juce::AudioProcessorValueTreeState apvts;

    std::unique_ptr<daisysp::Oscillator> oscillator;
    std::unique_ptr<daisysp::Tremolo> tremolo;

    // Wave shape visualizer
    WaveDrawBuffer waveDrawBuffer;
    WaveSampleCollector waveSampleCollector;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};
