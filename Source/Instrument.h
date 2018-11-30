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
enum ObjectType
{
    bowedString,
    plate,
};
class Instrument    : public Component
{
public:
    Instrument (vector<ObjectType> objectTypes, double fs);
    ~Instrument();

    void paint (Graphics&) override;
    void resized() override;
    
    vector<double> calculateOutput();
    
    OwnedArray<ViolinString>& getStrings() { return violinStrings; };
    OwnedArray<Plate>& getPlates() { return plates; };
    
    vector<Connection>& getConnections() { return connections; };
    
    int getNumStrings() { return numStrings; };
    int getNumPlates() { return numPlates; };
    
private:
    OwnedArray<ViolinString> violinStrings;
    OwnedArray<Plate> plates;
    vector<Connection> connections;
    double fs;
    unsigned int numStrings;
    unsigned int numPlates;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Instrument)
};
