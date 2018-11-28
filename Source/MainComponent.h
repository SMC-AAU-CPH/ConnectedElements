/*
 ==============================================================================
 
 This file was auto-generated!
 
 ==============================================================================
 */

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "ViolinString.h"
#include "Connection.h"
#include "../SenselWrapper/SenselWrapper.h"
//==============================================================================
/*
 This component lives inside our window, and this is where you should put all
 your controls and content.
 */
class MainComponent : public AudioAppComponent,
public HighResolutionTimer
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();
    
    //==============================================================================
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) override;
    void releaseResources() override;
    
    //==============================================================================
    void paint(Graphics &g) override;
    void resized() override;
    
    void mouseDown(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;
    void mouseDrag(const MouseEvent &e) override;
    void hiResTimerCallback() override;
    float clip(float output);
    
    class Connection {
    
    public:
        Connection(ViolinString* object1, ViolinString* object2,
                   double cp1, double cp2,
                   double width1, double width2,
                   double sx, double w0, double w1);
        ~Connection();
        
        double calculateJFc()[2];
        void calculateCoefs();
        
        vector<int> getCPIdx() { return cpIdx; };
        void setCP (int idx, double ratio) { cpIdx[idx] = floor(ratio * objects[idx]->getNumPoints()); };
        
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
    
private:
    //==============================================================================
    static double fs;
    double bufferSize;
    
    float minOut;
    float maxOut;
    
    OwnedArray<ViolinString> violinStrings;
    vector<Connection> connections;
    
    OwnedArray<Sensel> sensels;
    
    unsigned long stateUpdateCounter = 0;
    
    static const unsigned int amountOfSensels = 2;
    
    static const unsigned int numStrings = 2;

    vector<double> fp {0, 0};
    vector<double> bpX {0, 0};
    vector<double> bpY {0, 0};
    vector<double> cp {0, 0};
    
    array<float, amountOfSensels> force = {0.0};
    array<float, amountOfSensels> xpos = {0.0};
    array<float, amountOfSensels> ypos = {0.0};
    array<float, amountOfSensels> Vb = {0.0};
    array<float, amountOfSensels> Fb = {0.0};
    //array<float, amountOfSensels> fingerPoint = {0.0};
    array<float, amountOfSensels> connectionPoint = {0.0};
    array<bool, amountOfSensels> state = {0};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
