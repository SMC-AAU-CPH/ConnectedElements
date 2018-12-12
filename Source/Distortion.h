/*
  ==============================================================================

    Distortion.h
    Created: 12 Dec 2018 11:29:05am
    Author:  Nikolaj Schwab Andersson

  ==============================================================================
*/

#include "Clipper.hpp"
#include "Serge.hpp"

#pragma once

enum DistortionType
{ 
    DiodeClipper,
    Both,
    SergeVCM
};

class Distortion
{
  public: 
    void setSamplingRate(double fs);
    double getOutput(double input);
  private: 

    Clipper clipper; 
    SergeWavefolder serge[6];
    
    DistortionType dType = Both; 
    float mix = 0.0; 
};
