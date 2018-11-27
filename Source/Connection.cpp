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
                       double sx, double w0, double w1, double fs) : width1(width1), width2(width2),
                                                                     sx(sx), w0(w0), w1(w1), k (1.0 / fs)
{

    objects.push_back(object1);
    objects.push_back(object2);
    cpIdx.resize(2);
    cpIdx[0] = cp1 * object1->getNumPoints();
    cpIdx[1] = cp2 * object2->getNumPoints();
    
    hA = object1->getGridSpacing();
    hB = object2->getGridSpacing();
    s0A = object1->getS0();
    s0B = object2->getS0();
    
    JFc.resize(2);
    
}


void Connection::calculateCoefs()
{
    // Relative displacement
    etaR = hA * objects[0]->getStateAt(cpIdx[0]) - hB * objects[1]->getStateAt(cpIdx[1]);

    
    rn = (2*sx/k - w0*w0 - pow(w1,4) * etaR*etaR) / (2.0*sx/k + w0*w0 + pow(w1,4) * etaR*etaR);
    pn = -2.0/(2.0*sx/k + w0*w0 + pow(w1,4)*etaR*etaR);
}
    
vector<double> Connection::calculateJFc()
{
    bn = hA * objects[0]->getNextStateAt(cpIdx[0]) - hB * objects[1]->getNextStateAt(cpIdx[1]);
    an = rn * etaRPrev;
    
    jA = k * k / ((1 + s0A) * k);
    jB = -k * k * massRatio / ((1 + s0B) * k);
    
    etaRPrev = etaR;
    Fc = (an - bn) / ((hA * jA - hB * jB) - pn);
    
    JFc[0] = jA * Fc;
    JFc[1] = jB * Fc;
    
    return JFc;
}
