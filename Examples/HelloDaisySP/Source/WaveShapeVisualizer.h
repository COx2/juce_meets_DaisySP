#pragma once

#include <JuceHeader.h>

//==============================================================================
class WaveDrawBuffer
{
public:
    WaveDrawBuffer()
    {
        bufferToDraw.clear();
    }

    void push(const float* dataToPush, const int numSamples)
    {
        jassert(numSamples <= bufferToDraw.getNumSamples());

        int start1, size1, start2, size2;

        abstractFifo.prepareToWrite(1, start1, size1, start2, size2);

        if (size1 > 0)
            juce::FloatVectorOperations::copy(bufferToDraw.getWritePointer(start1), dataToPush, numSamples);

        abstractFifo.finishedWrite(size1);
    }

    void pop(float* outputBuffer, const int numSamples)
    {
        jassert(numSamples <= bufferToDraw.getNumSamples());

        int start1, size1, start2, size2;
        abstractFifo.prepareToRead(1, start1, size1, start2, size2);

        if (size1 > 0)
        {
            juce::FloatVectorOperations::copy(outputBuffer, bufferToDraw.getReadPointer(start1), numSamples);
            lastReadPointer = start1;
            resetCount = 0;
        }
        else
        {
            resetCount++;
            if (resetCount > resetThreshold)
                bufferToDraw.clear();

            juce::FloatVectorOperations::copy(outputBuffer, bufferToDraw.getReadPointer(lastReadPointer), numSamples);
        }

        abstractFifo.finishedRead(size1);
    }

    int getBufferSize() const
    {
        return bufferToDraw.getNumSamples();
    }

private:
    const int numBuffers{ 5 };
    const int bufferSize{ 1024 };
    int lastReadPointer{ 0 };
    const int resetThreshold{ 30 };
    int resetCount{ 0 };
    juce::AbstractFifo abstractFifo{ numBuffers };
    juce::AudioBuffer<float> bufferToDraw{ numBuffers, bufferSize };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveDrawBuffer)
};

class WaveSampleCollector
{
public:
    WaveSampleCollector(WaveDrawBuffer& bufferToUse)
        : drawBuffer(bufferToUse)
    {
        sampleCollecion.setSize(1, drawBuffer.getBufferSize());
    }

    void process(const float* samples, const int numSamples)
    {
        int index = 0;

        if (state == State::waitingForTrigger)
        {
            while (index++ < numSamples)
            {
                const auto currentSample = *samples++;

                if (currentSample >= triggerLevel && prevSample < triggerLevel)
                {
                    numCollected = 0;
                    state = State::collecting;
                    break;
                }

                prevSample = currentSample;
            }
        }

        if (state == State::collecting)
        {
            while (index++ < numSamples)
            {
                sampleCollecion.getWritePointer(0)[numCollected++] = *samples++;

                if (numCollected == drawBuffer.getBufferSize())
                {
                    drawBuffer.push(sampleCollecion.getReadPointer(0), sampleCollecion.getNumSamples());
                    state = State::waitingForTrigger;
                    prevSample = 100.0f;
                    numCollected = 0;
                    break;
                }
            }
        }
    }

private:
    enum class State
    {
        waitingForTrigger,
        collecting
    };
    State state{ State::waitingForTrigger };

    WaveDrawBuffer& drawBuffer;
    juce::AudioBuffer<float> sampleCollecion;
    int numCollected{ 0 };
    float prevSample = 100.0f;
    static constexpr float triggerLevel = 0.001f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveSampleCollector)
};
