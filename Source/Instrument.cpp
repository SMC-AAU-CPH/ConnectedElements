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
    
    double frequencyInHz[] = {110.0, 110.0 * pow(2, 7.0 / 12.0)};
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
            // plates.add (new Plate(...))
            addAndMakeVisible (plates[i]);
        }
    }
    numStrings = violinStrings.size();
    numPlates = plates.size();
    
//    connections.push_back(Connection (violinStrings[0], plates[0],
//                                      0.95, {0.95, 0.95},
//                                      1, 1,
//                                      1, 500, 10, fs));
//    violinStrings[0]->setConnection(0.95);
//    plates[0]->setConnection({0.95, 0.95});
}

Instrument::~Instrument()
{
}

void Instrument::paint (Graphics& g)
{
    g.setColour(Colours::orange);
//
//    double y1 = violinStrings[0]->getCy();
//    double y2 = plates[1]->getCy();
//
//    Line<float> connectionLine (ceil(connections[0].getCPIdx()[0] * getWidth() / objects[0]->getNumPoints()), y1,
//               ceil(connections[0].getCPIdx()[1] * getWidth() / objects[1]->getNumPoints()), getHeight() / 2.0 + y2);
//    float dashPattern[2];
//    dashPattern[0] = 3.0;
//    dashPattern[1] = 5.0;
//    g.drawDashedLine(connectionLine, dashPattern, 2, dashPattern[0], 0);
}

void Instrument::resized()
{
    violinStrings[0]->setBounds(0, 0, getWidth(), getHeight() / 2.0);
    plates[0]->setBounds(0, getHeight() / 2.0, getWidth(), getHeight() / 2.0);
}

Instrument::Connection::Connection (ViolinString* object1, ViolinString* object2,
                                       double cp1, double cp2,
                                       double width1, double width2,
                                       double sx, double w0, double w1, double fs) : width1(width1), width2(width2), sx(sx), w0(w0), w1(w1), k (1.0 / fs)
{
    violinStrings.push_back(object1);
    violinStrings.push_back(object2);
    cpIdx.resize(2);
    cpIdx[0] = cp1 * object1->getNumPoints();
    cpIdx[1] = cp2 * object2->getNumPoints();
    
    hA = object1->getGridSpacing();
    hB = object2->getGridSpacing();
    s0A = object1->getS0();
    s0B = object2->getS0();
    
    JFc.resize(2);
    
    jA = k * k / ((1 + s0A) * k);
    jB = -k * k * massRatio / ((1 + s0B) * k);
}

Instrument::Connection::Connection (ViolinString* object1, Plate* object2,
                                    double cp1, double cp2x, double cp2y,
                                    double width1, double width2,
                                    double sx, double w0, double w1, double fs) : width1(width1), width2(width2), sx(sx), w0(w0), w1(w1), k (1.0 / fs)
{
    violinStrings.push_back(object1);
    plates.push_back(object2);
    cpIdx.resize(2);
    cpIdx[0] = cp1 * object1->getNumPoints();
    cpIdx[1] = cp2x * object2->getNumXPoints();
    cpIdx[2] = cp2y * object2->getNumYPoints();
    
    hA = object1->getGridSpacing();
    hB = object2->getGridSpacing();
    s0A = object1->getS0();
    s0B = object2->getS0();
    
    JFc.resize(2);
    
    jA = k * k / ((1 + s0A) * k);
    jB = -k * k * massRatio / ((1 + s0B) * k);
}

vector<double> Instrument::calculateOutput()
{
    vector<double> output = {0.0, 0.0};
    
    // calculate coefficients from relative displacements
    for (int j = 0; j < connections.size(); ++j)
    {
        connections[j].calculateCoefs();
    }
    
    // Interact with the components
    for (int j = 0; j < numStrings; ++j)
    {
        violinStrings[j]->bow();
    }
    
    // Calculate the connection forces
//    for (int j = 0; j < connections.size(); ++j)
//    {
//        connections[j].calculateJFc();
//    }
    for (int j = 0; j < numStrings; ++j)
    {
//        violinStrings[j]->addJFc(connections[0].getJFc()[j], connections[0].getCPIdx()[j]);
        violinStrings[j]->updateUVectors();
        output[j] = violinStrings[j]->getOutput(0.75) * 600;
        output[j] = plates[j]->getOutput(output[j]);
    }
    return output;
}

void Instrument::Connection::calculateCoefs()
{
    // Relative displacement
//    etaR = hA * violinStrings[0]->getStateAt(cpIdx[0]) - hB * plates[0]->getStateAt(cpIdx[1]);
    
    rn = (2*sx/k - w0*w0 - pow(w1,4) * etaR*etaR) / (2.0*sx/k + w0*w0 + pow(w1,4) * etaR*etaR);
    pn = -2.0/(2.0*sx/k + w0*w0 + pow(w1,4)*etaR*etaR);
}

vector<double> Instrument::Connection::calculateJFc()
{
//    bn = hA * violinStrings[0]->getNextStateAt(cpIdx[0]) - hB * plates[0]->getNextStateAt(cpIdx[1]);
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
        if (e.y < getHeight() / 2.0)
        {
            violinStrings[0]->setBow(true);
        }
//        else
//        {
//            objects[1]->setBow(true);
//        }
    }
}

void Instrument::mouseDrag(const MouseEvent &e)
{
    double maxVb = 0.2;
    int idx = 0;
    if (e.y < getHeight() / 2.0)
    {
        if (ModifierKeys::getCurrentModifiers() == ModifierKeys::altModifier + ModifierKeys::leftButtonModifier)
        {
            cp[idx] = e.x <= 0 ? 0 : (e.x < getWidth() ? e.x / static_cast<double>(getWidth()) : 1);
            connections[0].setCP(idx, cp[idx]);
            violinStrings[idx]->setConnection (cp[idx]);
        }
        else if (ModifierKeys::getCurrentModifiers() == ModifierKeys::ctrlModifier + ModifierKeys::leftButtonModifier)
        {
            fp[idx] = e.x <= 0 ? 0 : (e.x < getWidth() ? e.x / static_cast<double>(getWidth()) : 1);
            violinStrings[idx]->setFingerPoint(fp[idx]);
        }
        else
        {
            double Vb = (e.y - getHeight() * (idx == 0 ? 0.25 : 0.75)) / (static_cast<double>(getHeight() * 0.25)) * maxVb;
            violinStrings[idx]->setVb(Vb);
            bpX[idx] = e.x <= 0 ? 0 : (e.x < getWidth() ? e.x / static_cast<double>(getWidth()) : 1);
            if (idx == 0)
            {
                bpY[idx] = e.y <= 0 ? 0 : (e.y < getHeight() / 2.0 ? e.y / static_cast<double>(getHeight() / 2.0) : 1);
            } else {
                bpY[idx] = e.y <= getHeight() / 2.0 ? 0 : (e.y < getHeight() ? (e.y - (getHeight() / 2.0)) / static_cast<double>(getHeight() / 2.0) : 1);
            }
            violinStrings[idx]->setBowPos(bpX[idx], bpY[idx]);
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
