/*
  ==============================================================================

    Instrument.h
    Created: 28 Nov 2018 11:29:52am
    Author:  Silvin Willemsen

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "ViolinString.h"
#include "Plate.h"

//==============================================================================
/*
*/
enum ObjectType
{
    bowedString,
    plate,
};

enum ConnectionType
{
    stringString,
    stringPlate,
    platePlate
};

class Instrument    : public Component
{
public:
    Instrument (vector<ObjectType> objectTypes, double fs);
    ~Instrument();

    void paint (Graphics&) override;
    void resized() override;
    
    class Connection {
        
    public:
        Connection(ViolinString* object1, ViolinString* object2,
                   double cp1, double cp2,
                   double width1, double width2,
                   double sx, double w0, double w1, double fs);

        Connection(ViolinString* object1, Plate* object2,
                   double cp1, double cp2x, double cp2y,
                   double width1, double width2,
                   double sx, double w0, double w1, double fs);
        
        void calculateCoefs();
        vector<double> calculateJFc();
        
//        void setCP (int idx, double ratio) { cpIdx[idx] = floor(ratio * violinStrings[idx]->getNumPoints()); };
//        void setCPPlate (int idx, double ratioX, double ratioY) { cpIdx[idx] = floor(ratioX * violinStrings[idx]->getNumPoints()); };
        
        vector<double> getJFc() { return JFc; };
        
        vector<int> connID;
        
        vector<ViolinString*> violinStrings;
        vector<Plate*> plates;
        
        ConnectionType connectionType;
        
    private:
        
        
        
        double width1, width2;  // Width of the connection
        double sx, w0, w1; // Spring constants: damping, linear spring constant, non-linear spring constant
        
        double etaRPrev = 0;
        double massRatio = 1;
        double hA, hB, s0A, s0B, Fc;
        double etaR, rn, pn, an, bn, jA, jB;
        vector<double> JFc;
        double k;
    };
    
    vector<double> calculateOutput();
    
    OwnedArray<ViolinString>& getStrings() { return violinStrings; };
    OwnedArray<Plate>& getPlates() { return plates; };
    
    vector<Connection>& getConnections() { return connections; };
    
    void mouseDown(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;
    void mouseDrag(const MouseEvent &e) override;
    
    int getNumStrings() { return numStrings; };
    int getNumPlates() { return numPlates; };
    
private:
    OwnedArray<ViolinString> violinStrings;
    OwnedArray<Plate> plates;
    vector<Connection> connections;
    double fs;
    unsigned int numStrings;
    unsigned int numPlates;
    
    vector<double> fp;
    vector<double> bpX;
    vector<double> bpY;
    vector<double> cp;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Instrument)
};
