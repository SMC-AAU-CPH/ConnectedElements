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
               double sx, double w0, double w1);
    double calculateJFc(double k);
private:
    ViolinString* object1;
    ViolinString* object2;
    int cpIdx1, cpIdx2;
    double width1, width2;  // Width of the connection
    double sx, w0, w1; // Spring damping, linear spring constant, non-linear spring constant
    
    double etaRPrev = 0;
    double massRatio = 1;
    double Fc;
};
