#include "PluginProcessor.h"
#include "PluginEditor.h"



//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
    , apvts(*this, nullptr, "PARAMETERS", createParameterLayout())
    , waveSampleCollector(waveDrawBuffer)
{
    oscillator = std::make_unique<daisysp::Oscillator>();
    tremolo = std::make_unique<daisysp::Tremolo>();
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::ignoreUnused (sampleRate, samplesPerBlock);

    oscillator->Init(sampleRate);
    tremolo->Init(sampleRate);
}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.

    oscillator->Reset();
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void AudioPluginAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Apply oscillator parameter
    {
        const auto* wave_param = apvts.getParameter("Waveform");
        const WaveformType wave_type = (WaveformType)wave_param->getNormalisableRange().convertFrom0to1(wave_param->getValue());
        switch (wave_type)
        {
        case kSine:
            oscillator->SetWaveform(daisysp::Oscillator::WAVE_SIN);
            break;
        case kSquare:
            oscillator->SetWaveform(daisysp::Oscillator::WAVE_SQUARE);
            break;
        case kTriangle:
            oscillator->SetWaveform(daisysp::Oscillator::WAVE_TRI);
            break;
        case kSaw:
            oscillator->SetWaveform(daisysp::Oscillator::WAVE_SAW);
            break;
        }

        const auto* freq_param = apvts.getParameter("Frequency");
        const float frequency = freq_param->getNormalisableRange().convertFrom0to1(freq_param->getValue());
        oscillator->SetFreq(frequency);

        const auto* gain_param = apvts.getParameter("Gain");
        const float gain = gain_param->getNormalisableRange().convertFrom0to1(gain_param->getValue());
        oscillator->SetAmp(gain);
    }

    // Apply tremolo parameter
    {
        const auto* wave_param = apvts.getParameter("TremoloWaveform");
        const WaveformType wave_type = (WaveformType)wave_param->getNormalisableRange().convertFrom0to1(wave_param->getValue());
        switch (wave_type)
        {
        case kSine:
            tremolo->SetWaveform(daisysp::Oscillator::WAVE_SIN);
            break;
        case kSquare:
            tremolo->SetWaveform(daisysp::Oscillator::WAVE_SQUARE);
            break;
        case kTriangle:
            tremolo->SetWaveform(daisysp::Oscillator::WAVE_TRI);
            break;
        case kSaw:
            tremolo->SetWaveform(daisysp::Oscillator::WAVE_SAW);
            break;
        }

        const auto* freq_param = apvts.getParameter("TremoloFrequency");
        const float frequency = freq_param->getNormalisableRange().convertFrom0to1(freq_param->getValue());
        tremolo->SetFreq(frequency);

        const auto* depth_param = apvts.getParameter("TremoloDepth");
        const float depth = depth_param->getNormalisableRange().convertFrom0to1(depth_param->getValue());
        tremolo->SetDepth(depth);
    }

    for (int sample_idx = 0; sample_idx < buffer.getNumSamples(); sample_idx++)
    {
        float sample_data = oscillator->Process();
        sample_data = tremolo->Process(sample_data);

        for (int ch_idx = 0; ch_idx < buffer.getNumChannels(); ch_idx++)
        {
            buffer.getWritePointer(ch_idx)[sample_idx] = sample_data;
        }
    }

    // Collect wave sample
    waveSampleCollector.process(buffer.getReadPointer(0), buffer.getNumSamples());
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout AudioPluginAudioProcessor::createParameterLayout() const
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add(std::make_unique<juce::AudioParameterFloat>("Gain", "Gain", juce::NormalisableRange<float>{ 0.0f, 1.0f, 0.01f }, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Frequency", "Frequency", juce::NormalisableRange<float>{ 20.0f, 2000.0f, 1.0f }, 440.0f));
    layout.add(std::make_unique<juce::AudioParameterChoice>("Waveform", "Waveform", waveformTypes, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>("TremoloFrequency", "TremoloFrequency", juce::NormalisableRange<float>{ 20.0f, 2000.0f, 1.0f }, 20.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("TremoloDepth", "TremoloDepth", juce::NormalisableRange<float>{ 0.0f, 1.0f, 0.01f }, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterChoice>("TremoloWaveform", "TremoloWaveform", waveformTypes, 0));
    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}
