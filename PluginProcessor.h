/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
class BitCrush_Parameter : public AudioProcessorParameter
{

public:

    float defaultValue{0};
    float currentValue{0};
    String name;

    float getValue() const override
    {
        return currentValue;
    }

    void setValue (float newValue) override
    {
        currentValue = newValue;
    }

    float getDefaultValue () const override
    {
        return defaultValue;
    }

    String getName (int maximumStringLength) const override
    {
        return name;
    }

    String getLabel () const override
    {
        return getName(10);
    }

    float getValueForText (const String &text) const override
    {
        return 1;
    }
};

//==============================================================================
/**
*/
class MashrAudioProcessor  : public AudioProcessor
{
    AudioSampleBuffer noiseBuffer, currentOutputBuffer;

    BitCrush_Parameter* noiseAmount;
    BitCrush_Parameter* rateRedux;
    BitCrush_Parameter* bitRedux;
public:
    //==============================================================================
    MashrAudioProcessor();
    ~MashrAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;


    void fillDelayBuffer (int channel, const int bufferLength, const int delayBufferLength,
                          const float* bufferData, const float* delayBufferData);

    void getFromDelayBuffer (AudioBuffer<float>& buffer, int channel, const int bufferLength,
                             const int delayBufferLength, const float* bufferData, const float* delayBufferData);

    void feedbackDelay (int channel, const int bufferLength,
                        const int delayBufferLength, float* dryBuffer);

    int delayTime { 1000 };
    double feedbackAmt {0.5};
    bool delayGate = false;

    void updateFilter();
    void updateParameters();


    float drive {1};

    //AudioBuffer<float> audioBufferData;
    AudioBuffer<float> audioBufferData;


private:
    float lastSampleRate;
    AudioBuffer<float> mDelayBuffer;
    int mWritePosition { 0 };
    int mSampleRate { 44100 };

    dsp::ProcessorDuplicator<dsp::StateVariableFilter::Filter<float>, dsp::StateVariableFilter::Parameters<float>> stateVariableFilter;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MashrAudioProcessor)
};
