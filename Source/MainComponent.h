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
#include "VUMeter.h"

//==============================================================================
/*
 This component lives inside our window, and this is where you should put all
 your controls and content.
 */

class MainComponent : public AudioAppComponent,
                      public HighResolutionTimer,
                      public Timer,
                      public Slider::Listener,
                      private MidiInputCallback,
                      private MidiKeyboardStateListener
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
    
    void sliderValueChanged (Slider* slider) override;
    
    // MIDI
    
    void setMidiInput (int index);
    
    static String getMidiMessageDescription (const MidiMessage& m)
    {
        if (m.isNoteOn())           return "Note on "          + MidiMessage::getMidiNoteName (m.getNoteNumber(), true, true, 3);
        if (m.isNoteOff())          return "Note off "         + MidiMessage::getMidiNoteName (m.getNoteNumber(), true, true, 3);
        if (m.isProgramChange())    return "Program change "   + String (m.getProgramChangeNumber());
        if (m.isPitchWheel())       return "Pitch wheel "      + String (m.getPitchWheelValue());
        if (m.isAftertouch())       return "After touch "      + MidiMessage::getMidiNoteName (m.getNoteNumber(), true, true, 3) +  ": " + String (m.getAfterTouchValue());
        if (m.isChannelPressure())  return "Channel pressure " + String (m.getChannelPressureValue());
        if (m.isAllNotesOff())      return "All notes off";
        if (m.isAllSoundOff())      return "All sound off";
        if (m.isMetaEvent())        return "Meta event";
        
        if (m.isController())
        {
            String name (MidiMessage::getControllerName (m.getControllerNumber()));
            
            if (name.isEmpty())
                name = "[" + String (m.getControllerNumber()) + "]";
            
            return "Controller " + name + ": " + String (m.getControllerValue());
        }
        
        return String::toHexString (m.getRawData(), m.getRawDataSize());
    }
    
    
    void handleIncomingMidiMessage (MidiInput* source, const MidiMessage& message) override;
    
    void handleNoteOn (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;
    
    void handleNoteOff (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;
    
    void setPitch();
    
    double clamp (double input, double min, double max);
    
private:
    void senselMappingTwoStrings();
    void senselMappingSitarBowed();
    void senselMappingSitarPlucked();
    void senselMappingHurdyGurdy();
    void senselMappingDulcimer();
    //==============================================================================
    double fs;
    double bufferSize;
    
    float minOut;
    float maxOut;

    OwnedArray<Instrument> instruments;
    int numInstruments;
    OwnedArray<Sensel> sensels;
    
    unsigned long stateUpdateCounter = 0;
    static const InstrumentType chooseInstrument = dulcimer;
    static const unsigned int amountOfSensels = (chooseInstrument == hurdyGurdy) ? 1 : 2;
    
    Distortion dist {None};
    
    int appWidth = 1440;
    int appHeight = 800;
    int controlsWidth = 100;
    
    
    OwnedArray<Slider> mixSliders;
    Slider* plateStiffness;
    OwnedArray<Label> labels;
    Label* plateLabel;
    Slider* exciterLengthSlider;
    Label* exciterLengthLabel;
    
    // MIDI
    MidiKeyboardState keyboardState;
    MidiKeyboardComponent keyboardComponent;
    
    AudioDeviceManager deviceManager;           // [1]
    ComboBox midiInputList;                     // [2]
    Label midiInputListLabel;
    int lastInputIndex = 0;                     // [3]
    bool isAddingFromMidiInput = false;         // [4]
    
    TextEditor midiMessagesBox;
    double startTime;
    
    vector<bool> midiNotesBool;
    double pitchOffset = 0;
    int numDroneStrings = 2;
    
    //flip sensels
    bool flip = (chooseInstrument == twoStringViolin || chooseInstrument == bowedSitar) ? false : true;
    
    VUMeter vuMeter;
    double outputLvlLeft = 0;
    double outputLvlRight = 0;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

