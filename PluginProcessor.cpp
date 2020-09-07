/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================


  //Code excerpts lifted from Joshua Hodge's (TheAudioProgrammer) Bitcrusher Demo.





*/

//#include <juce_dsp/processors/juce_ProcessContext.h>
//#include <juce_dsp/processors/juce_ProcessorDuplicator.h>
#include "PluginProcessor.h"
#include "PluginEditor.h"

static Array<float> getSimpleNoise(int numSamples)
{
    Random r = Random::getSystemRandom();
    Array<float> noise;

    for (int s=0; s < numSamples; s++)
    {
        noise.add((r.nextFloat() - .5)*2);
    }

    return noise;

}

static Array<float> getWhiteNoise(int numSamples) {

    Array<float> noise;

    float z0 = 0;
    float z1 = 0;
    bool generate = false;

    float mu = 0; // center (0)
    float sigma = 1; // spread -1 <-> 1

    float output = 0;
    float u1 = 0;
    float u2 = 0;

    Random r = Random::getSystemRandom();
    r.setSeed(Time::getCurrentTime().getMilliseconds());
    const float epsilon = std::numeric_limits<float>::min();

    for (int s=0; s < numSamples; s++)
    {

        // GENERATE ::::
        // using box muller method
        // https://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform
        generate = !generate;

        if (!generate)
            output =  z1 * sigma + mu;
        else
        {
            do
            {
                u1 = r.nextFloat();
                u2 = r.nextFloat();
            }
            while ( u1 <= epsilon );

            z0 = sqrtf(-2.0 * logf(u1)) * cosf(2*float(double_Pi) * u2);
            z1 = sqrtf(-2.0 * logf(u1)) * sinf(2*float(double_Pi) * u2);

            output = z0 * sigma + mu;
        }

        // NAN check ...
        jassert(output == output);
        jassert(output > -50 && output < 50);

        //
        noise.add(output);

    }

    return noise;

}
//==============================================================================
MashrAudioProcessor::MashrAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ) //tree(*this, nullptr), lowPass(dsp::IIR::Coefficients<float>::makeLowPass(44100,20000, 0.1))
#endif

{




    noiseAmount = new BitCrush_Parameter();
    noiseAmount->defaultValue = 0;
    noiseAmount->currentValue = 0;
    noiseAmount->name = "Noise";
    addParameter(noiseAmount);

    rateRedux = new BitCrush_Parameter();
    rateRedux->defaultValue = 1;
    rateRedux->currentValue = 1;
    rateRedux->name = "Rate";
    addParameter(rateRedux);


    bitRedux = new BitCrush_Parameter();
    bitRedux->defaultValue = 32;
    bitRedux->currentValue = 32;
    bitRedux->name = "Bits";
    addParameter(bitRedux);
}

MashrAudioProcessor::~MashrAudioProcessor()
{
}

//==============================================================================
const String MashrAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MashrAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MashrAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MashrAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MashrAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MashrAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MashrAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MashrAudioProcessor::setCurrentProgram (int index)
{
}

const String MashrAudioProcessor::getProgramName (int index)
{
    return {};
}

void MashrAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void MashrAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{

    const int numInputChannels = getTotalNumInputChannels();
    const int delayBufferSize = 2 * (sampleRate + samplesPerBlock);
    mSampleRate = sampleRate;

    //delay buffer setup
    mDelayBuffer.setSize(numInputChannels, delayBufferSize);


}

void MashrAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}



#ifndef JucePlugin_PreferredChannelConfigurations
bool MashrAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif



void MashrAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());



    const int bufferLength = buffer.getNumSamples();
    const int delayBufferLength = mDelayBuffer.getNumSamples();



    int numSamples = buffer.getNumSamples();

    float noiseAmt = -120 + 120*(noiseAmount->getValue()/100); // dB
    float bitDepth = bitRedux->getValue();
    int rateDivide = rateRedux->getValue();

    // GET PARAMETERS :::::
    noiseAmt = jlimit<float>(-120, 0, noiseAmt);
    noiseAmt = Decibels::decibelsToGain(noiseAmt);



    // SAFETY CHECK :::: since some hosts will change buffer sizes without calling prepToPlay (Bitwig)
    if (noiseBuffer.getNumSamples() != numSamples)
    {
        noiseBuffer.setSize(2, numSamples, false, true, true); // clears
        currentOutputBuffer.setSize(2, numSamples, false, true, true); // clears
    }


    // COPY for processing ...
    currentOutputBuffer.copyFrom(0, 0, buffer.getReadPointer(0), numSamples);
    if (buffer.getNumChannels() > 1) currentOutputBuffer.copyFrom(1, 0, buffer.getReadPointer(1), numSamples);



    // BUILD NOISE ::::::
    {
        noiseBuffer.clear();

        Array<float> noise = getWhiteNoise(numSamples);

        // range bound
        noiseAmt = jlimit<float>(0, 1, noiseAmt);

        FloatVectorOperations::multiply(noise.getRawDataPointer(), noiseAmt, numSamples);

        // ADD the noise ...
        FloatVectorOperations::add(noiseBuffer.getWritePointer(0), noise.getRawDataPointer(), numSamples);
        FloatVectorOperations::add(noiseBuffer.getWritePointer(1), noise.getRawDataPointer(), numSamples); // STEREO

        // MULTIPLY MODE :::::
        // Multiply the noise by the signal ... so 0 signal -> 0 noise
        //        {
        //            FloatVectorOperations::multiply(noiseBuffer.getWritePointer(0), currentOutputBuffer.getWritePointer(0), numSamples);
        //            FloatVectorOperations::multiply(noiseBuffer.getWritePointer(1), currentOutputBuffer.getWritePointer(1), numSamples);
        //        }

    }


    // ADD NOISE to the incoming AUDIO ::::
    currentOutputBuffer.addFrom(0, 0, noiseBuffer.getReadPointer(0), numSamples);
    currentOutputBuffer.addFrom(1, 0, noiseBuffer.getReadPointer(1), numSamples);



    // RESAMPLE AS NEEDED :::::
    for (int chan=0; chan < currentOutputBuffer.getNumChannels(); chan++)
    {

        float* data = currentOutputBuffer.getWritePointer(chan);

        for (int i=0; i < numSamples; i++)
        {
            // REDUCE BIT DEPTH :::::
            float totalQLevels = powf(2, bitDepth);
            float val = data[i];
            float remainder = fmodf(val, 1/totalQLevels);

            // Quantize ...
            data[i] = val - remainder;

            if (rateDivide > 1)
            {
                if (i%rateDivide != 0) data[i] = data[i - i%rateDivide];
            }
//
            audioBufferData = currentOutputBuffer;


        }
    }




    // COPY to the actual output buffer :::
    buffer.copyFrom(0, 0, currentOutputBuffer, 0, 0, numSamples);
    buffer.copyFrom(1, 0, currentOutputBuffer, 1, 0, numSamples);

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
        for (int sample = 0; sample < buffer.getNumSamples(); sample++){
            // multiply samples from channelData by drive variable
            *channelData *= drive;
            //  By Reon Fourie
            //  Using mathematical function as opposed to just cliiping values at 1.
            //  This function (2/PI * atan(x)) creates a smooth line that approaches 1 from beneath.

            *channelData = (2.f / float_Pi) * atan(*channelData);
//            *filter->processSamples (buffer.getWritePointer (i), buffer.getNumSamples());




            channelData++;





        }
        const float* bufferData = buffer.getReadPointer(channel);
        const float* delayBufferData = mDelayBuffer.getReadPointer(channel);
        float* dryBuffer = buffer.getWritePointer(channel);


        //Copying delay buffer to output buffer
        fillDelayBuffer(channel, bufferLength, delayBufferLength, bufferData, delayBufferData);
        getFromDelayBuffer(buffer, channel, bufferLength, delayBufferLength, bufferData, delayBufferData);
        feedbackDelay(channel, bufferLength, delayBufferLength, dryBuffer);


        //hard clip output stream
        if(*channelData > 1.0f){
        auto channelData = 1.0f;
        }
        else if(*channelData < -1.0f){
            auto channelData = -1.0f;
        }

    }
    mWritePosition += bufferLength;
    mWritePosition %= delayBufferLength;



}

void MashrAudioProcessor::fillDelayBuffer(int channel, const int bufferLength, const int delayBufferLength,
                                             const float* bufferData, const float* delayBufferData){

    //code excerpt by Joshua Hodge.  Fills a delay buffer with values from audio buffer.

    if(delayBufferLength > bufferLength + mWritePosition){
        mDelayBuffer.copyFromWithRamp(channel, mWritePosition, bufferData, bufferLength, 0.8, 0.8);
    }
    else{
        const int bufferRemaining = delayBufferLength - mWritePosition;
        mDelayBuffer.copyFromWithRamp(channel, mWritePosition, bufferData, bufferRemaining, 0.8, 0.8);
        mDelayBuffer.copyFromWithRamp(channel, 0, bufferData, bufferLength - bufferRemaining, 0.8, 0.8);

    }

}

void MashrAudioProcessor::getFromDelayBuffer(AudioBuffer<float>& buffer, int channel,
                                                const int bufferLength, const int delayBufferLength, const float* bufferData, const float* delayBufferData){


    //code excerpt by Joshua Hodge.  Takes data from delay buffer, and copies it to the output buffer.
    //delayTime = 500;
    const int readPosition = static_cast<int> (delayBufferLength + mWritePosition - ( mSampleRate * delayTime /1000)) % delayBufferLength;

    if (delayBufferLength > bufferLength + readPosition){
        buffer.copyFrom(channel, 0, delayBufferData + readPosition, bufferLength );
    }
    else{
        //Ensures copying loops back to beginning of the buffer when the delay buffer has more values
        // left to copy than buffer's length
        const int bufferRemaining = delayBufferLength - readPosition;
        buffer.copyFrom(channel, 0, delayBufferData + readPosition, bufferRemaining);
        buffer.copyFrom(channel, bufferRemaining, delayBufferData, bufferLength - bufferRemaining);
    }

}

void MashrAudioProcessor::feedbackDelay(int channel, const int bufferLength,
                                           const int delayBufferLength, float* dryBuffer){

    //Delay Feedback loop

    if (delayBufferLength > bufferLength + mWritePosition){
        mDelayBuffer.addFromWithRamp(channel, mWritePosition, dryBuffer, bufferLength, feedbackAmt, feedbackAmt);
    }
    else{
        //Similar technique as getFromDelayBuffer
        const int bufferRemaining = delayBufferLength - mWritePosition;
        mDelayBuffer.addFromWithRamp(channel, bufferRemaining, dryBuffer, bufferRemaining, feedbackAmt, feedbackAmt);
        mDelayBuffer.addFromWithRamp(channel, 0, dryBuffer, bufferLength - bufferRemaining, feedbackAmt, feedbackAmt);
    }

}

//==============================================================================
bool MashrAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* MashrAudioProcessor::createEditor()
{
    return new MashrAudioProcessorEditor (*this);
}

//==============================================================================
void MashrAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void MashrAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MashrAudioProcessor();
}
