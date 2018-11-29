/*
  ==============================================================================

    Instrument.cpp
    Created: 28 Nov 2018 11:29:52am
    Author:  Silvin Willemsen

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "Instrument.h"

//==============================================================================
Instrument::Instrument (vector<ObjectType> objectTypes, double fs) : fs (fs)
{
    
    double frequencyInHz[] = {110.0, 110.0 * pow(2, 12.0 / 12.0), 110.0 * pow(2, (12.0 + 7.0) / 12.0), 115.0 * pow(2, 12.0 / 12.0), 115 * pow(2, (12.0 + 7.0) / 12.0)};
    
    // TODO counter for individual objecttypes
    for (int i = 0; i < objectTypes.size(); ++i)
    {
        if (objectTypes[i] == bowedString)
        {
            violinStrings.add (new ViolinString(frequencyInHz[i], fs));
            addAndMakeVisible (violinStrings[i]);
        }
        else if (objectTypes[i] == plate)
        {
            plates.add (new Plate(fs));
            addAndMakeVisible (plates[0]);
        }
    }
    numStrings = violinStrings.size();
    numPlates = plates.size();
  /*
    connections.push_back(Connection (violinStrings[0], plates[0],
                                      0.5, 0.7, 0.7,
                                      1, 1,
                                      1, 1000, 100, fs));*/
    
    connections.push_back(Connection (violinStrings[0], violinStrings[1],
                                      0.5, 0.5,
                                      1, 1,
                                      1, 1000, 100, fs));
    connections.push_back(Connection (violinStrings[1], violinStrings[2],
                                      0.1, 0.5,
                                      1, 1,
                                      1, 1000, 100, fs));
    
    connections.push_back(Connection (violinStrings[2], violinStrings[3],
                                      0.1, 0.5,
                                      1, 1,
                                      1, 1000, 100, fs));
    
    connections.push_back(Connection (violinStrings[3], violinStrings[4],
                                      0.1, 0.5,
                                      1, 1,
                                      1, 10000, 100, fs));
    
    /*
    connections.push_back(Connection (violinStrings[numStrings-1], plates[0],
                                      0.1, 0.3, 0.3,
                                      1, 1,
                                      1, 10000, 4000, fs));
     */
    
}

Instrument::~Instrument()
{
}

void Instrument::paint (Graphics& g)
{
    g.setColour(Colours::orange);
//    double y1;
//    double y2;
//    int cpX1;
//    int cpX2;
//    
//    int stringIdx = 0;
//    for (int i = 0; i < connections.size(); ++i)
//    {
//        if (connections[i].connectionType == stringString)
//        {
//
//            cpX1 = connections[i].violinStrings[0]->getCP(connections[i].connID[0]);
//            cpX2 = connections[i].violinStrings[1]->getCP(connections[i].connID[1]);
//            y1 = connections[i].violinStrings[0]->getCy(cpX1);
//            y2 = connections[i].violinStrings[1]->getCy(cpX2);
//            Line<float> connectionLine (ceil(cpX1 * getWidth() / connections[i].violinStrings[0]->getNumPoints()), y1,
//                                        ceil(cpX2 * getWidth() / connections[i].violinStrings[1]->getNumPoints()), stringIdx * getHeight() / static_cast<double>(numStrings) + y2);
//            ++stringIdx;
//            float dashPattern[2];
//            dashPattern[0] = 3.0;
//            dashPattern[1] = 5.0;
//            g.drawDashedLine(connectionLine, dashPattern, 2, dashPattern[0], 0);
//        }
//    }
}

void Instrument::resized()
{
    for (int i = 0; i < numStrings; ++i)
    {
        violinStrings[i]->setBounds(0 , i * ((getHeight() / 2.0) / static_cast<double>(numStrings)), getWidth(), (getHeight() / 2.0) / static_cast<double>(numStrings));
    }
    plates[0]->setBounds(0, getHeight() / 2.0, getWidth(), getHeight() / 2.0);
}

Instrument::Connection::Connection (ViolinString* object1, ViolinString* object2,
                                       double cp1, double cp2,
                                       double width1, double width2,
                                       double sx, double w0, double w1, double fs) : width1(width1), width2(width2), sx(sx), w0(w0), w1(w1), k (1.0 / fs)
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

Instrument::Connection::Connection (ViolinString* object1, Plate* object2,
                                    double cp1, double cp2x, double cp2y,
                                    double width1, double width2,
                                    double sx, double w0, double w1, double fs) : width1(width1), width2(width2), sx(sx), w0(w0), w1(w1), k (1.0 / fs)
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

vector<double> Instrument::calculateOutput()
{
    vector<double> output = {0.0, 0.0};
    
    // calculate coefficients from relative displacements
    for (int c = 0; c < connections.size(); ++c)
    {
        connections[c].calculateCoefs();
    }
    
    // Interact with the components
    for (auto violinString : violinStrings)
        violinString->bow();
    
    for (auto plate : plates)
        plate->excite();
    
    // Calculate the connection forces
    for (int c = 0; c < connections.size(); ++c)
    {
        connections[c].calculateJFc();
        if (connections[c].connectionType == stringString)
        {
            connections[c].violinStrings[0]->addJFc(connections[c].getJFc()[0], connections[c].violinStrings[0]->getCP(connections[c].connID[0]));
            connections[c].violinStrings[1]->addJFc(connections[c].getJFc()[1], connections[c].violinStrings[1]->getCP(connections[c].connID[1]));
        } else if (connections[c].connectionType == stringPlate)
        {
            connections[c].violinStrings[0]->addJFc(connections[c].getJFc()[0], connections[c].violinStrings[0]->getCP(connections[c].connID[0]));
            connections[c].plates[0]->addJFc (connections[c].getJFc()[1], connections[c].plates[0]->getCP(connections[c].connID[1]));
        }
    }
    
    for (auto violinString : violinStrings)
        violinString->updateUVectors();
    
    for (auto plate : plates)
        plate->updateUVectors();
    
    
    for (int i = 0; i < violinStrings.size(); i++)
    {
        float volume = 1;
        //volume *= 5;
        output[0] +=  violinStrings[i]->getOutput(0.5) * 600 * volume;
    }
    
    //output[0] += 0.1 * plates[0]->getOutput(0.3, 0.4) * 3;
    output[1] = output[0];
    
    //output[0] = violinStrings[2]->getOutput(0.75) * 600 + 0.1 * plates[0]->getOutput(0.3, 0.4) * 3;
    //output[1] = violinStrings[3]->getOutput(0.75) * 600 + 0.1 * plates[0]->getOutput(0.7, 0.4) * 3;
    
    return output;
}

void Instrument::Connection::calculateCoefs()
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

vector<double> Instrument::Connection::calculateJFc()
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

void Instrument::mouseDown(const MouseEvent &e)
{
    if (ModifierKeys::getCurrentModifiers() == ModifierKeys::leftButtonModifier)
    {
        for (int i = 0; i < numStrings; ++i)
        {
            if (e.y >= i * (getHeight() / 2.0) / static_cast<double>(numStrings) && e.y < (i + 1) * (getHeight() / 2.0) / static_cast<double>(numStrings))
            {
                violinStrings[i]->setBow(true);
            }
        }
    }
}

void Instrument::mouseDrag(const MouseEvent &e)
{
    double maxVb = 0.2;
    for (int i = 0; i < numStrings; ++i)
    {
        if (e.y >= i * (getHeight() / 2.0) / static_cast<double>(numStrings) && e.y < (i + 1) * (getHeight() / 2.0) / static_cast<double>(numStrings))
        {
    //        if (ModifierKeys::getCurrentModifiers() == ModifierKeys::altModifier + ModifierKeys::leftButtonModifier)
    //        {
    //            cp[idx] = e.x <= 0 ? 0 : (e.x < getWidth() ? e.x / static_cast<double>(getWidth()) : 1);
    ////            connections[0].setCP(idx, cp[idx]);
    ////            violinStrings[idx]->setConnection (cp[idx]);
    //        }
    //        else if (ModifierKeys::getCurrentModifiers() == ModifierKeys::ctrlModifier + ModifierKeys::leftButtonModifier)
    //        {
    //            fp[idx] = e.x <= 0 ? 0 : (e.x < getWidth() ? e.x / static_cast<double>(getWidth()) : 1);
    //            violinStrings[idx]->setFingerPoint(fp[idx]);
    //        }
    //        else
    //        {
    //        double Vb = (e.y - getHeight() * (idx == 0 ? 0.25 : 0.75)) / (static_cast<double>(getHeight() * 0.25)) * maxVb;
            double Vb = maxVb;
            violinStrings[i]->setVb(Vb);
            bpX[i] = e.x <= 0 ? 0 : (e.x < getWidth() ? e.x / static_cast<double>(getWidth()) : 1);
            if (i == 0)
            {
                bpY[i] = e.y <= 0 ? 0 : (e.y < getHeight() / 4.0 ? e.y / static_cast<double>(getHeight() / 4.0) : 1);
            } else {
                bpY[i] = e.y <= getHeight() / 4.0 ? 0 : (e.y < getHeight() ? (e.y - (getHeight() / 4.0)) / static_cast<double>(getHeight() / 4.0) : 1);
            }
            violinStrings[i]->setBowPos(bpX[i], bpY[i]);
        }
    }
}

void Instrument::mouseUp(const MouseEvent &e)
{
    for (auto violinString : violinStrings)
    {
        violinString->setBow(false);
    }
}
