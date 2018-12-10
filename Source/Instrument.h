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
    Instrument (vector<ObjectType> objectTypes, double fs, int stringPlateDivision);
    ~Instrument();

    void paint (Graphics&) override;
    void resized() override;
    
    vector<double> calculateOutput();
    
    OwnedArray<ViolinString>& getStrings() { return violinStrings; };
    OwnedArray<Plate>& getPlates() { return plates; };
    
    vector<Connection>& getConnections() { return connections; };
    
    int getNumBowedStrings() { return numBowedStrings; };
    int getNumSympStrings() { return numSympStrings; };
    int getNumPlates() { return numPlates; };
    
private:
    OwnedArray<ViolinString> violinStrings;
    OwnedArray<Plate> plates;
    vector<Connection> connections;
    double fs;
    unsigned int numBowedStrings = 0;
    unsigned int numSympStrings = 0;
    unsigned int numPlates = 0;
    int stringPlateDivision;
    int sympStringHeight = 50;
    int totBowedStringHeight;
    int totSympStringHeight;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Instrument)
};
