/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "Images.h"

//==============================================================================

class Visualiser : public AudioVisualiserComponent
{
public:
    Visualiser() : AudioVisualiserComponent(2){
        setBufferSize(512);
        setSamplesPerBlock(256);
        setColours(Colours::white, Colours::black);
    }

};
/**
*/
class MashrAudioProcessorEditor  : public AudioProcessorEditor, public Slider::Listener
{
public:
    MashrAudioProcessorEditor (MashrAudioProcessor&);
    ~MashrAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    Slider bitRedux, rateRedux, noise, driveSlider;

    //AudioVisualiserComponent visualiser;



private:

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MashrAudioProcessor& processor;
    void sliderValueChanged (Slider* slider) override;
    Slider delSlider;
    Slider feedSlider;
    Image backgroundIMG;





    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MashrAudioProcessorEditor)
};
