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

//==============================================================================
/*
*/

class Instrument    : public Component
{
public:
    Instrument (vector<String> objectNames, double fs);
    ~Instrument();

    void paint (Graphics&) override;
    void resized() override;
    
    class Connection {
        
    public:
        Connection(ViolinString* object1, ViolinString* object2,
                   double cp1, double cp2,
                   double width1, double width2,
                   double sx, double w0, double w1, double fs);
        
        vector<double> calculateJFc();
        void calculateCoefs();
        
        vector<int> getCPIdx() { return cpIdx; };
        void setCP (int idx, double ratio) { cpIdx[idx] = floor(ratio * objects[idx]->getNumPoints()); };
        
        vector<double> getJFc() { return JFc; };
        
    private:
        vector<ViolinString*> objects;
        
        vector<int> cpIdx;
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
    
    OwnedArray<ViolinString>& getObjects() { return objects; };
    vector<Connection>& getConnections() { return connections; };
    
    void mouseDown(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;
    void mouseDrag(const MouseEvent &e) override;
    
private:
    OwnedArray<ViolinString> objects;
    vector<Connection> connections;
    double fs;
    unsigned int numStrings = 2;
    
    vector<double> fp {0, 0};
    vector<double> bpX {0, 0};
    vector<double> bpY {0, 0};
    vector<double> cp {0, 0};
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Instrument)
};
