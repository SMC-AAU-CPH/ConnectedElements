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
    
    void setRaisedCos (double exciterPos, double width, double force);
    void pick(bool pick) {_isPicking = pick;};
    bool isPicking() { return _isPicking; };
    
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
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ViolinString)
};

class StringExciter
{
public:
    StringExciter()
    {
        Fe.resize(maxLength);
        
        for (int i = 0; i < exciterLength; i++)
            Fe[i] = Fmax / 2.0f * (1 - cos(2 * M_PI * i / exciterLength));
        
        /*for (int i = 0; i < exciterLength; i++)
         std::cout << "exciter: " << Fe[i] << "\n";*/
    }
    
    void excite()
    {
        play = true;
    }
    void setLength(int L)
    {
        if (L > maxLength)
            L = maxLength;
        
        exciterLength = L;
        
        for (int i = 0; i < exciterLength; i++)
            Fe[i] = Fmax * 0.5f * (1.0f - cos(q * M_PI * i / exciterLength));
    }
    
    void setLevel(double level)
    {
        //Fmax = level * 1e8;
        
        for (int i = 0; i < exciterLength; i++)
            Fe[i] = Fmax * 0.5f * (1.0f - cos(q * M_PI * i / exciterLength));
    }
    
    double getOutput()
    {
        double output = 0.0f;
        
        if (play)
        {
            output = Fe[pos];
            //std::cout << "exciter: " << Fe[pos] << "\n";
            pos++;
        }
        
        if (pos >= exciterLength)
        {
            pos = 0;
            play = false;
        }
        return output;
    }
    
    bool play = false;
    int pos = 0;
    float q = 1;
    int exciterLength = 10;
    int maxLength = 2000;
    double Fmax = 1.0f;
    
    vector<double> Fe;
};
