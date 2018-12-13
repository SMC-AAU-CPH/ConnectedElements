/*
 ==============================================================================
 
 This file was auto-generated!
 
 ==============================================================================
 */

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "../SenselWrapper/SenselWrapper.h"
#include "Instrument.h"
#include "Distortion.h"

//==============================================================================
/*
 This component lives inside our window, and this is where you should put all
 your controls and content.
 */

class MainComponent : public AudioAppComponent,
                      public HighResolutionTimer,
                      public Timer
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();
    
    //==============================================================================
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) override;
    void releaseResources() override;
    
    //==============================================================================
    void paint(Graphics &g) override;
    void resized() override;

    void hiResTimerCallback() override;
    void timerCallback() override;

    float clip(float output);
    
private:
    //==============================================================================
    double fs;
    double bufferSize;
    
    float minOut;
    float maxOut;

    OwnedArray<Instrument> instruments;
    int numInstruments;
    OwnedArray<Sensel> sensels;
    
    unsigned long stateUpdateCounter = 0;
    static const unsigned int amountOfSensels = 2;
    
    // Sensel variables
    // array<float, amountOfSensels> force = {0.0};
    // array<float, amountOfSensels> xpos = {0.0};
    // array<float, amountOfSensels> ypos = {0.0};
    // array<float, amountOfSensels> Vb = {0.0};
    // array<float, amountOfSensels> Fb = {0.0};
    // array<float, amountOfSensels> fp = {0.0};
    // array<float, amountOfSensels> connectionPoint = {0.0};
    // array<bool, amountOfSensels> state = {0};
    Distortion dist {None}; 
    int chooseInstrument;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
