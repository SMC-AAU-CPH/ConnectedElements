/*
  ==============================================================================

    Connection.cpp
    Created: 30 Nov 2018 11:19:29am
    Author:  Silvin Willemsen

  ==============================================================================
*/

#include "Connection.h"
Connection::Connection (ViolinString* object1, ViolinString* object2,
                                    double cp1, double cp2,
                                    double width1, double width2,
                                    double sx, double w0, double w1,
                                    double massRatio, double fs) : width1(width1), width2(width2), sx(sx), w0(w0), w1(w1), massRatio(massRatio), k(1.0 / fs)
{
    violinStrings.push_back(object1);
    violinStrings.push_back(object2);
    
    // Both adding a connection as well as retrieving its index (how manyth connection on that object)
    connID.push_back(object1->addConnection (cp1));
    connID.push_back(object2->addConnection (cp2));
    
    hA = object1->getGridSpacing();
    hB = object2->getGridSpacing();
    s0A = object1->getS0();
    s0B = object2->getS0();
    
    JFc.resize(2);
    
    jA = k * k / ((1 + s0A) * k);
    jB = -k * k * massRatio / ((1 + s0B) * k);
    
    connectionType = stringString;
}

Connection::Connection (ViolinString* object1, Plate* object2,
                                    double cp1, double cp2x, double cp2y,
                                    double width1, double width2,
                                    double sx, double w0, double w1,
                                    double massRatio, double fs) : width1(width1), width2(width2), sx(sx), w0(w0), w1(w1), massRatio(massRatio), k(1.0 / fs)
{
    violinStrings.push_back(object1);
    plates.push_back(object2);
    
    connID.push_back(object1->addConnection (cp1));
    connID.push_back(object2->addConnection (make_tuple(cp2x, cp2y)));
    
    hA = object1->getGridSpacing();
    hB = object2->getGridSpacing();
    s0A = object1->getS0();
    s0B = object2->getS0();
    
    JFc.resize(2);
    
    jA = k * k / ((1 + s0A) * k);
    jB = -k * k * massRatio / ((1 + s0B) * k);
    
    connectionType = stringPlate;
}



void Connection::calculateCoefs()
{
    // Relative displacement
    int cp1 = violinStrings[0]->getCP (connID[0]);
    if (connectionType == stringString)
    {
        int cp2 = violinStrings[1]->getCP (connID[1]);
        etaR = hA * violinStrings[0]->getStateAt(cp1) - hB * violinStrings[1]->getStateAt(cp2);
    } else if (connectionType == stringPlate)
    {
        auto cp2 = plates[0]->getCP (connID[1]);
        etaR = hA * violinStrings[0]->getStateAt(cp1) - hB * plates[0]->getStateAt(cp2);
    }
    
    rn = (2*sx/k - w0*w0 - pow(w1,4) * etaR*etaR) / (2.0*sx/k + w0*w0 + pow(w1,4) * etaR*etaR);
    pn = -2.0/(2.0*sx/k + w0*w0 + pow(w1,4)*etaR*etaR);
}

vector<double> Connection::calculateJFc()
{
    if(connectionType == stringString)
    {
        bn = hA * violinStrings[0]->getNextStateAt(violinStrings[0]->getCP(connID[0])) - hB * violinStrings[1]->getNextStateAt(violinStrings[1]->getCP(connID[1]));
    } else if (connectionType == stringPlate)
    {
        bn = hA * violinStrings[0]->getNextStateAt(violinStrings[0]->getCP(connID[0])) - hB * plates[0]->getNextStateAt(plates[0]->getCP(connID[1]));
    }
    an = rn * etaRPrev;
    
    etaRPrev = etaR;
    Fc = (an - bn) / ((hA * jA - hB * jB) - pn);
    
    JFc[0] = jA * Fc;
    JFc[1] = jB * Fc;
    
    return JFc;
}
