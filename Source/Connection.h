/*
  ==============================================================================

    Connection.h
    Created: 30 Nov 2018 11:19:29am
    Author:  Silvin Willemsen

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

#include "ViolinString.h"
#include "Plate.h"

using namespace std;

enum ConnectionType
{
    bowedStringBowedString,
    bowedStringPluckedString,
    bowedStringSympString,
    bowedStringPlate,
    pluckedStringPluckedString,
    pluckedStringSympString,
    pluckedStringPlate,
    sympStringPlate,
    sympStringSympString,
    platePlate
};

class Connection {
    
public:
    Connection(ViolinString* object1, ViolinString* object2,
               double cp1, double cp2,
               double width1, double width2,
               double sx, double w0, double w1,
               double massRatio, double fs);
    
    Connection(ViolinString* object1, Plate* object2,
               double cp1, double cp2x, double cp2y,
               double width1, double width2,
               double sx, double w0, double w1,
               double massRatio, double fs);
    
    void calculateCoefs();
    vector<double> calculateJFc();
    
    vector<double> getJFc() { return JFc; };
    
    vector<int> connID;
    
    vector<ViolinString*> violinStrings;
    vector<Plate*> plates;
    
    ConnectionType connectionType;
    
    void setConnectionType (ObjectType type1, ObjectType type2);
    
private:
    
    double width1, width2;  // Width of the connection
    double sx, w0, w1; // Spring constants: damping, linear spring constant, non-linear spring constant
    double massRatio; // Ratio of mass between object1 and object2
    
    double phiMin, phiPlus, powW1;


    double etaRPrev = 0;
    double hA, hB, s0A, s0B, Fc;
    double etaR, rn, pn, an, bn, jA, jB;
    vector<double> JFc;
    double k;
    double massDiff;
};
