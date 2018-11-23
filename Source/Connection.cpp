/*
  ==============================================================================

    Connection.cpp
    Created: 22 Nov 2018 6:16:29pm
    Author:  Nikolaj Schwab Andersson

  ==============================================================================
*/

#include "Connection.h"
Connection::Connection()
{
    
}

Connection::Connection(ViolinString* object1, ViolinString* object2,
                       double cp1, double cp2,
                       double width1, double width2,
                       double sx, double w0, double w1) : object1(object1), object2(object2),
                                                          width1(width1), width2(width2),
                                                          sx(sx), w0(w0), w1(w1)
{

    cpIdx1 = cp1 * object1->getNumPoints();
    cpIdx2 = cp2 * object2->getNumPoints();
//    for (int i = 0; i < width1; ++i)
//    {
//        cpIdx1[i] = (1 - cos (2 * double_Pi * i / width1)) * 0.5;
//    }
//    for (int i = 0; i < width2; ++i)
//    {
//        cpIdx2[i] = (1 - cos (2 * double_Pi * i / width2)) * 0.5;
//    }
    
}

double Connection::calculateJFc(double k)
{
    // Relative displacement
    double hA = object1->getGridSpacing();
    double hB = object2->getGridSpacing();
    double etaR = hA * object1->getPrevStateAt(cpIdx1)
            - hB * object2->getPrevStateAt(cpIdx1);
    
    double rn = (2*sx/k - w0*w0 - pow(w1,4) * etaR*etaR) / (2.0*sx/k + w0*w0 + pow(w1,4) * etaR*etaR);
    double pn = -2.0/(2.0*sx/k + w0*w0 + pow(w1,4)*etaR*etaR);
    
    double bn = hA * object1->getStateAt(cpIdx1) - hB * object2->getStateAt(cpIdx1);
    double an = rn * etaRPrev;
    
    double s0A = object1->getS0();
    double s0B = object2->getS0();
    
    double jA = k * k / ((1 + s0A) * k);
    double jB = -k * k * massRatio / ((1 + s0A) * k);
    
    etaRPrev = etaR;
    Fc = (an - bn) / ((hA * jA + hB * jB) - pn);
    
    
    
    return Fc;
}
