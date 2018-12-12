/*
  ==============================================================================

    Distortion.h
    Created: 12 Dec 2018 11:29:05am
    Author:  Nikolaj Schwab Andersson

  ==============================================================================
*/

#include "Clipper.hpp"
#include "Serge.hpp"
#include <atomic>

#pragma once

enum DistortionType
{ 
    None, 
    Both,
    DiodeClipper,
    SergeVCM
};

using namespace std;

class Distortion
{
  public: 
    void setSamplingRate(double fs);
    double getOutput(double input);

    void setDrive(double drive) {gain = drive;}; 
    void setMix(double m) {mix = m;}; 

  private: 

    Clipper clipper; 
    SergeWavefolder serge[6];
    
    DistortionType dType = Both;
    atomic<float> gain {5.0};
    atomic<float> mix {0.5}; 
};
