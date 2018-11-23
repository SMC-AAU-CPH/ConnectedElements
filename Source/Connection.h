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
    Connection(ViolinString* object1, ViolinString* object2,
               double cp1, double cp2,
               double width1, double width2,
               double sx, double w0, double w1);
    
private:
    OwnedArray<ViolinString> objects;
    double cp1, cp2, width1, width2, sx, w0, w1; //Spring damping, linear spring constant, non-linear spring constant
};
