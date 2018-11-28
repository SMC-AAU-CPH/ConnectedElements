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

class Connection : public Component
{
public:
    Connection();
    Connection(ViolinString* object1, ViolinString* object2,
               double cp1, double cp2,
               double width1, double width2,
               double sx, double w0, double w1, double fs);
    
    void paint(Graphics& g) override;
    void resized() override;
    
    void setCoeffs(ViolinString* object1, ViolinString* object2,
                   double cp1, double cp2,
                   double width1, double width2,
                   double sx, double w0, double w1, double fs);
    
    vector<double> calculateJFc();
    void calculateCoefs();
    
    vector<int> getCPIdx() { return cpIdx; };
    void setCP (int idx, double ratio) { cpIdx[idx] = floor(ratio * objects[idx]->getNumPoints()); };
//    void mouseDrag (const MouseEvent& e) override
//    {
//        std::cout << "Connection " << e.x << std::endl;
//    }
private:
    vector<ViolinString*> objects;
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
