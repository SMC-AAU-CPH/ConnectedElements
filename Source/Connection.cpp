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
                       double sx, double w0, double w1, double fs)
{
    
    //setInterceptsMouseClicks (false, false);
    setCoeffs (object1, object2, cp1, cp2, width1, width2, sx, w0, w1, fs);
//    addMouseListener(getParentComponent(), true);
}

void Connection::setCoeffs(ViolinString* object1, ViolinString* object2,
                           double cp1, double cp2,
                           double wid1, double wid2,
                           double sxDamp, double w0Spring, double w1Spring, double fs)
{
    width1 = wid1;
    width2 = wid2;

    sx = sxDamp;
    w0 = w0Spring;
    w1 = w1Spring;
    k  = 1.0 / fs;

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

void Connection::paint(Graphics& g)
{
//    std::cout << "painting" << std::endl;
    g.setColour(Colours::orange);
    g.drawLine(ceil(cpIdx[0] * getWidth() / objects[0]->getNumPoints()), getHeight() / 4.0,
               ceil(cpIdx[1] * getWidth() / objects[1]->getNumPoints()), getHeight() * 3.0 / 4.0, 5);
    
}

void Connection::resized()
{
    std::cout << "resized" << std::endl;
}
