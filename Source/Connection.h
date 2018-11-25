/*
  ==============================================================================

    Connection.h
    Created: 22 Nov 2018 6:16:29pm
    Author:  Nikolaj Schwab Andersson

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "ViolinString.h"

using namespace std;

class Connection
{
public:
    Connection();
    Connection(ViolinString* object1, ViolinString* object2,
               double cp1, double cp2,
               double width1, double width2,
               double sx, double w0, double w1, double k);
    vector<double> calculateJFc();
    void calculateCoefs();
    
    vector<int> getCPIdx() { return cpIdx; };
    void setCPIdx1 (int idx) { cpIdx[0] = idx; };
    void setCPIdx2 (int idx) { cpIdx[1] = idx; };
    
private:
    ViolinString* object1;
    ViolinString* object2;
    vector<int> cpIdx;
    double width1, width2;  // Width of the connection
    double sx, w0, w1; // Spring damping, linear spring constant, non-linear spring constant
    
    double etaRPrev = 0;
    double massRatio = 1;
    double hA, hB, s0A, s0B, Fc;
    double etaR, rn, pn, an, bn, jA, jB;
    vector<double> JFc;
    
    double k;
};
