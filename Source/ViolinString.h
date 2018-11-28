/*
  ==============================================================================

    ViolinString.h
    Created: 14 Nov 2018 3:10:58pm
    Author:  Silvin Willemsen

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
using namespace std;
//==============================================================================
/*
*/
class ViolinString : public Component, public Timer
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

//    void setFrequency(double freq);
    void setFingerPoint(double val) { fp = floor (val * N); };
    void setFingerForce(double val) { ff = val; };
    //void setFingerOn (bool val) { fingerOn = val; };

    void setBow(bool val) { _isBowing.store(val); };
    void setVb(double val) { _Vb.store(val); }
    void setFb(double val) { _Fb.store(val); }
    void setBowPos(double bpX, double bpY) { _bpX.store(bpX); _bpY.store(bpY); };
    void setFingerOn (bool val) { fingerOn = val; };
    
    double getNumPoints() { return N; };
    double getPrevStateAt (int idx) { return uPrev[idx]; };
    double getStateAt (int idx) { return u[idx]; };
    double getNextStateAt (int idx) { return uNext[idx]; };
    
    vector<double>& getState() { return u; };
    double getGridSpacing() { return h; };
    double getS0() { return s0; };
    
    
    void updateUVectors();
    
    void setRaisedCos (double exciterPos, double width);
    void setRaisedCosSinglePoint (double exciterPos);
    
    Path generateStringPathAdvanced();
    
    void setConnection (double cp) { _cpIdx.store(floor(cp * N)); };
    
    void timerCallback() override;
    void mouseDrag(const MouseEvent& e) override;
    
private:
    double fs, freq, gamma, k, s0, s1, B, kappa, h, N, lambdaSq, muSq, kOh, gOh, a, BM, tol, q, qPrev, b, eps, fp;
    double ff = 0.7;
    atomic<double> _Vb, _Fb, _bpX, _bpY;
    atomic<int> _cx, _cy, _cpIdx;
    atomic<bool> _isBowing{false};

    vector<double> u, uPrev, uNext;
    bool active = false;

    unsigned long count;
    unsigned long t = 0;
    
    bool fingerOn = false;
    int fpx = 0;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ViolinString)
};
