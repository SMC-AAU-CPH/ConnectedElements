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

    a1 = 2*sx/k - w0*w0;
    b1 = 2.0*sx/k + w0*w0;
    powW1 = pow(w1,4);
    
    setConnectionType (object1->getStringType(), object2->getStringType());
    
}

Connection::Connection (ViolinString* object1, Plate* object2,
                                    double cp1, double cp2x, double cp2y,
                                    double width1, double width2,
                                    double sx, double w0, double w1,
                                    double massRatio, double fs) : width1(width1), width2(width2), sx(sx), w0(w0), w1(w1), massRatio(massRatio), k (1.0 / fs)
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
    
    a1 = 2*sx/k - w0*w0;
    b1 = 2.0*sx/k + w0*w0;
    powW1 = pow(w1,4);

    setConnectionType (object1->getStringType(), plate);
}



void Connection::calculateCoefs()
{
    // Relative displacement
    int cp1 = violinStrings[0]->getCP (connID[0]);
    switch (connectionType)
    {
        case bowedStringBowedString:
        case bowedStringPluckedString:
        case bowedStringSympString:
        case pluckedStringPluckedString:
        case pluckedStringSympString:
        case sympStringSympString:
        {
            int cp2 = violinStrings[1]->getCP (connID[1]);
            etaR = hA * violinStrings[0]->getStateAt(cp1) - hB * violinStrings[1]->getStateAt(cp2);
            break;
        }
        case bowedStringPlate:
        case pluckedStringPlate:
        case sympStringPlate:
        {
            auto cp2 = plates[0]->getCP (connID[1]);
            etaR = hA * violinStrings[0]->getStateAt(cp1) - hB * plates[0]->getStateAt(cp2);
            break;
        }
        case platePlate:
            break;
    }
    //a1 = 2*sx/k - w0*w0;
    //b1 = 2.0*sx/k + w0*w0;
    //powW1 = pow(w1,4);
    double etaSq = etaR*etaR;
    //double b2 = 2.0*sx/k + w0*w0 + pow(w1,4);
    //rn = (2*sx/k - w0*w0 - pow(w1,4) * etaR*etaR) / (2.0*sx/k + w0*w0 + pow(w1,4) * etaR*etaR);
    //pn = -2.0/(2.0*sx/k + w0*w0 + pow(w1,4)*etaR*etaR);
    rn = (a1 - powW1 * etaSq) / (b1 + powW1 *etaSq);
    pn = -2.0/(b1 + powW1 * etaSq);
}

vector<double> Connection::calculateJFc()
{
    switch (connectionType)
    {
        case bowedStringBowedString:
        case bowedStringPluckedString:
        case bowedStringSympString:
        case pluckedStringPluckedString:
        case pluckedStringSympString:
        case sympStringSympString:
            bn = hA * violinStrings[0]->getNextStateAt(violinStrings[0]->getCP(connID[0])) - hB * violinStrings[1]->getNextStateAt(violinStrings[1]->getCP(connID[1]));
            break;
        case bowedStringPlate:
        case pluckedStringPlate:
        case sympStringPlate:
            bn = hA * violinStrings[0]->getNextStateAt(violinStrings[0]->getCP(connID[0])) - hB * plates[0]->getNextStateAt(plates[0]->getCP(connID[1]));
            break;
        case platePlate:
            break;
    }
    an = rn * etaRPrev;
    
    etaRPrev = etaR;
    Fc = (an - bn) / ((hA * jA - hB * jB) - pn);
    
    JFc[0] = jA * Fc;
    JFc[1] = jB * Fc;
    
    return JFc;
}

void Connection::setConnectionType (ObjectType type1, ObjectType type2)
{
    switch (type1) {
        case bowedString:
        {
            switch (type2) {
                case bowedString:
                    connectionType = bowedStringBowedString;
                    break;
                case pluckedString:
                    connectionType = bowedStringPluckedString;
                    break;
                case sympString:
                    connectionType = bowedStringSympString;
                    break;
                case plate:
                    connectionType = bowedStringPlate;
                    break;
            }
            break;
        }
        case pluckedString:
        {
            switch (type2) {
                case pluckedString:
                    connectionType = pluckedStringPluckedString;
                    break;
                case sympString:
                    connectionType = pluckedStringSympString;
                    break;
                case plate:
                    connectionType = pluckedStringPlate;
                    break;
            }
            break;
        }
        case sympString:
        {
            switch (type2) {
                case sympString:
                    connectionType = sympStringSympString;
                    break;
                case plate:
                    connectionType = sympStringPlate;
                    break;
            }
            break;
        }
        case plate:
            connectionType = platePlate;
            break;
    }
}
