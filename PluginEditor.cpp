/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MashrAudioProcessorEditor::MashrAudioProcessorEditor (MashrAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    backgroundIMG = ImageCache::getFromMemory(Images::mashrBackground_png, Images::mashrBackground_pngSize);
    getLookAndFeel().setColour(Slider::trackColourId, Colours::black);

    getLookAndFeel().setColour(Slider::thumbColourId, Colours::white);

    //setup for sliders

    noise.setName("Noise Amount");
    noise.setRange(0, 100); // %
    noise.setValue(processor.getParameters()[0]->getValue());
    noise.setSliderStyle(Slider::RotaryVerticalDrag);
    noise.setColour(Slider::textBoxTextColourId, Colours::white);
    noise.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, false, 200, 20);
    noise.setValue(0);

    noise.addListener(this);
    addAndMakeVisible(&noise);

    bitRedux.setName("Bits");
    bitRedux.setRange(2, 32
    );
    bitRedux.setValue(processor.getParameters()[0]->getValue());
    bitRedux.setSliderStyle(Slider::RotaryVerticalDrag);
    bitRedux.setColour(Slider::textBoxTextColourId, Colours::white);
    bitRedux.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, false, 200, 20);
    bitRedux.setValue(32);
    bitRedux.addListener(this);
    addAndMakeVisible(&bitRedux);

    rateRedux.setName("Rate");
    rateRedux.setRange(1, 50); // division rate (rate / x)
    rateRedux.setValue(processor.getParameters()[0]->getValue());
    rateRedux.setSliderStyle(Slider::RotaryVerticalDrag);
    rateRedux.setColour(Slider::textBoxTextColourId, Colours::white);
    rateRedux.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, false, 200, 20);
    rateRedux.setValue(1);
    rateRedux.addListener(this);
    addAndMakeVisible(&rateRedux);

    driveSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 100,25);
    driveSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    driveSlider.addListener(this);
    driveSlider.setRange(1.0, 30.0);
    driveSlider.setValue(0.0);
    addAndMakeVisible(driveSlider);

    delSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 100,25);
    delSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    delSlider.addListener(this);
    delSlider.setRange(500, 5000);
    delSlider.setValue(500);
    addAndMakeVisible(delSlider);

    feedSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 100,25);
    feedSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    feedSlider.addListener(this);
    feedSlider.setRange(0.1, 0.9);
    feedSlider.setValue(0.0);
    addAndMakeVisible(feedSlider);







    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (600, 600);
}

MashrAudioProcessorEditor::~MashrAudioProcessorEditor()
{
}
//send slider values to variables
void MashrAudioProcessorEditor::sliderValueChanged(Slider *slider) {
    if ( slider == &noise)
    {
        processor.getParameters()[0]->setValue(slider->getValue());
    }
    else if ( slider == &rateRedux)
    {
        processor.getParameters()[1]->setValue(slider->getValue());

    }
    else if ( slider == &bitRedux)
    {
        processor.getParameters()[2]->setValue(slider->getValue());

    }
    else if ( slider ==&driveSlider){
        processor.drive = driveSlider.getValue();
    }
    else if ( slider ==&delSlider){
        processor.delayTime = delSlider.getValue();
    }
    else if ( slider ==&feedSlider){
        processor.feedbackAmt = feedSlider.getValue();
    }


}

//==============================================================================
void MashrAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::white.withMultipliedBrightness(1));
    //draw background
    g.drawImage(backgroundIMG,0,0,600,600,0,0,600,600);

    g.setColour(Colours::white);

    int labelW = 100;

}

void MashrAudioProcessorEditor::resized()
{
    int margin = 10;
    int w = 60;
    int y = 50;

    //draw sliders
    noise.setBounds(40,15,100,100);

    bitRedux.setBounds(40,128,100,100);

    rateRedux.setBounds(42,248,100,100);

    driveSlider.setBounds(380,85,200,200);

    delSlider.setBounds(42,367,100,100);
    feedSlider.setBounds(42,482,100,100);


}


