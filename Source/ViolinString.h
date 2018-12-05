/*
  ==============================================================================

    ViolinString.h
    Created: 14 Nov 2018 3:10:58pm
    Author:  Silvin Willemsen

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
//#include "Object.h"

enum InterpolationType
{
    noInterpolation,
    linear,
    cubic,
};

using namespace std;
//==============================================================================
/*
*/
class ViolinString : public Component
{
public:
    ViolinString(double freq, double fs);
    ~ViolinString();
    
    void paint(Graphics &) override;
    void resized() override;
    
    void reset(); 
    void bow();
    void addJFc (double JFc, int index);
    
    void newtonRaphson();
    double getOutput (double ratio);

    void setFingerForce(double val) { ff = val; };
    void setFingerPosition(double fingerPos){ fp = clamp(fingerPos, 0, 1); };
    void setBow(bool val) { _isBowing.store(val); };
    void setVb(double val) { _Vb.store(val); }
    void setFb(double val) { _Fb.store(val); }
    void setBowPos(double bpX, double bpY) { _bpX.store(bpX); _bpY.store(bpY); };
    void setFingerOn (bool val) { fingerOn = val; };
    
    double getNumPoints() { return N; };
    double getPrevStateAt (int idx) { return uPrev[idx]; };
    double getStateAt (int idx) { return u[idx]; };
    double getNextStateAt (int idx) { return uNext[idx]; };
    
//    vector<double>& getState() { return u; };
    double getGridSpacing() { return h; };
    double getS0() { return s0; };
    
    void updateUVectors();
    
    void setRaisedCos (double exciterPos, double width);
    void setRaisedCosSinglePoint (double exciterPos);
    
    Path generateStringPathAdvanced();
    
    void setConnection (int idx, double cp) {
        if(cpIdx.size())
            cpIdx[idx] = clamp(floor(cp * N), 2, N-2);
    }
    int addConnection (double cp);
    
    int getCP (int idx) { return cpIdx[idx]; };
    int getCy (int idx) { return cy[idx]; };
    
    double clamp (double input, double min, double max);
    
    void mouseDown (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;
    
private:
    double fs, freq, gamma, k, s0, s1, B, kappa, h, N, lambdaSq, muSq, kOh, gOh, a, BM, tol, q, qPrev, b, eps, fp;
    double ff = 0.7;
    atomic<double> _Vb, _Fb, _bpX, _bpY;
    
    vector<int> cx, cy, cpIdx;
    atomic<bool> _isBowing{false};

    double* uNext;
    double* u;
    double* uPrev;
    int uNextPtrIdx = 0;
    vector<vector<double>> uVecs;
    bool active = false;

    unsigned long count;
    unsigned long t = 0;
    
    bool fingerOn = false;
    int fpx = 0;
    
    InterpolationType interpolation = cubic;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ViolinString)
};
