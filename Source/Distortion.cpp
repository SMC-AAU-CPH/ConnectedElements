/*
  ==============================================================================

    Distortion.cpp
    Created: 12 Dec 2018 11:29:05am
    Author:  Nikolaj Schwab Andersson

  ==============================================================================
*/

#include "Distortion.h"

void Distortion::setSamplingRate(double fs)
{
  for (int i = 0; i < 6; i++)
    serge[i].setSamplingRate(fs);

  clipper.setSamplingRate(fs);
}

double Distortion::getOutput(double input)
{
  double out = 0;

  double outD = 0; // Diode-clipper
  double outS = 0; // serge

  float drive = 5;

  if (dType == DiodeClipper || dType == Both)
    outD = clipper.process(input * drive);

  if (dType == SergeVCM || dType == Both)
  {
    outS = serge[0].process(input * drive);
    for (int i = 1; i < 6; i++)
      outS = serge[i].process(outS);
  }
  //outS *= 4;
  out = mix * outD + (1 - mix) * outS;

  return out;
}
