/*
  ==============================================================================

    Instrument.h
    Created: 28 Nov 2018 11:29:52am
    Author:  Silvin Willemsen

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Connection.h"

//==============================================================================
/*
*/

class Instrument    : public Component
{
public:
    Instrument (InstrumentType instrumentType, vector<ObjectType> objectTypes, double fs, int stringPlateDivision, int bowedSympDivision);
    ~Instrument();

    void paint (Graphics&) override;
    void resized() override;
    
    vector<double> calculateOutput();
    
    OwnedArray<ViolinString>& getStrings() { return violinStrings; };
    OwnedArray<Plate>& getPlates() { return plates; };
    
    vector<Connection>& getConnections() { return connections; };
    
    int getNumBowedStrings() { return numBowedStrings; };
    int getNumPluckedStrings() { return numPluckedStrings; };
    int getNumSympStrings() { return numSympStrings; };
    
    int getTotNumStrings() { return numBowedStrings + numPluckedStrings + numSympStrings; };
    int getNumPlates() { return numPlates; };
    
    void setMix (int idx, double val) { mix[idx] = val; };
    
private:
    OwnedArray<ViolinString> violinStrings;
    OwnedArray<Plate> plates;
    vector<Connection> connections;
    double fs;
    unsigned int numBowedStrings = 0;
    unsigned int numPluckedStrings = 0;
    unsigned int numSympStrings = 0;
    unsigned int numPlates = 0;
    
    int stringPlateDivision;
    int bowedSympDivision;
    int totBowedStringHeight;
    int totSympStringHeight;
    
    InstrumentType instrumentType;
    
    vector<double> mix {0.0, 0.0};
    double mixVal;
    Random r; 
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Instrument)
};
