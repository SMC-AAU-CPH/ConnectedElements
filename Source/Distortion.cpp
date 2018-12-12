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

  double outS = 0; // serge

  // add dc offset
  //double cv_dc = ((inputs[CV_DC].value / 5) + 1) * 0.5;
  //in += params[DC_PARAM].value * cv_dc;


  outS = serge[0].process(input*5);

  for (int i = 1; i < 6; i++)
    outS = serge[i].process(outS);


   //outS *= 4;  
   
   return clipper.process(outS);

}
