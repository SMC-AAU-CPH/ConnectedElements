/*
  ==============================================================================

    ViolinString.h
    Created: 14 Nov 2018 3:10:58pm
    Author:  Silvin Willemsen

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "ObjectTypes.h"
#include "Exciter.h"

enum StringInterpolType
{
    noStringInterpol,
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
    ViolinString(double freq, double fs, ObjectType stringType, int stringID);
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
    void setVb(double val) { _Vb = val; };
    void setFb(double val) { _Fb = val; };
    void setBowPos(double bpX, double bpY) { _bpX = bpX; _bpY = bpY; };
    void setFingerOn (bool val) { fingerOn = val; };
    
    double getNumPoints() { return N; };
    double getPrevStateAt (int idx) { return uPrev[idx]; };
    double getStateAt (int idx) { return u[idx]; };
    double getNextStateAt (int idx) { return uNext[idx]; };
    
//    vector<double>& getState() { return u; };
    double getGridSpacing() { return h; };
    double getS0() { return s0; };
    
    void updateUVectors();
    
    void setRaisedCos (double exciterPos, double width, double force);
    void pick(bool pick) {_isPicking = pick; stringExciter.excite();};
    bool isPicking() { return _isPicking;};
    
    Path generateStringPathAdvanced();
    
    void setConnection (int idx, double cp);
    int addConnection (double cp);
    
    int getCP (int idx) { return cpIdx[idx]; };
    int getCy (int idx) { return cy[idx]; };
    
    double clamp (double input, double min, double max);
    
    void mouseDown (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;
    
    double linearInterpolation (double* uVec, int bowPosition, double alpha);
    double cubicInterpolation (double* uVec, int bowPosition, double alpha);
    
    ObjectType getStringType() { return stringType; };
    int getStringID() { return stringID; };
    
private:
    double fs, freq, gamma, k, s0, s1, B, kappa, h, N, lambdaSq, muSq, kOh, gOh, a, BM, tol, q, qPrev, b, eps, fp;
    double ff = 0.7;
    atomic<double> _Vb, _Fb, _bpX, _bpY;
    
    double bowPos;
    
    vector<int> cx;
    vector<int> cy;
    vector<int> cpIdx;
    atomic<bool> _isBowing{false};
    atomic<bool> _isPicking{false};

    double* uNext;
    double* u;
    double* uPrev;
    int uNextPtrIdx = 0;
    vector<vector<double>> uVecs;
    
    //NR interpolation variables
    double uI = 0;
    double uIPrev = 0;
    double uI1 = 0;
    double uI2 = 0;
    double uIM1 = 0;
    double uIM2 = 0;
    double uIPrev1 = 0;
    double uIPrevM1 = 0;
    
    bool active = false;

    unsigned long count;
    unsigned long t = 0;
    
    bool fingerOn = false;
    int fpx = 0;
    
    StringInterpolType interpolation = cubic;
    int cpMoveIdx = -1;
    int cpMR = 10; //connection point move range
    
    ObjectType stringType;
    int stringID;
    
    bool pluck = false;
    int pluckIdx = 0;
    
    int exciterStart = 0;
    int exciterEnd = 0;
    double exciterForce = 0;
    
    StringExciter stringExciter;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ViolinString)
};


