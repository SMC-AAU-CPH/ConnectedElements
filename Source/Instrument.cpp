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
    
    vector<double> frequencyInHz = {110.0, 110.0 * pow(2, 7.0 / 12.0), 110.0 * pow(2, (14.0) / 12.0), 110.0 * pow(2, 21.0 / 12.0), 110 * pow(2, (28.0) / 12.0)};
    
    // TODO counter for individual objecttypes
    for (int i = 0; i < objectTypes.size(); ++i)
    {
        if (objectTypes[i] == bowedString)
        {
            if (i > frequencyInHz.size() - 1)
                frequencyInHz.push_back(frequencyInHz[frequencyInHz.size()-1]);
            
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

//    connections.push_back(Connection (violinStrings[0], plates[0],
//                                      0.5, 0.7, 0.7,
//                                      1, 1,
//                                      1, 1000, 100,
//                                      0.25, fs));
//
//    connections.push_back(Connection (violinStrings[0], violinStrings[1],
//                                      0.5, 0.5,
//                                      1, 1,
//                                      1, 1000, 100,
//                                      1, fs));
//
//    connections.push_back(Connection (violinStrings[1], violinStrings[2],
//                                      0.1, 0.5,
//                                      1, 1,
//                                      1, 1000, 100,
//                                      1, fs));
//
//    connections.push_back(Connection (violinStrings[2], violinStrings[3],
//                                      0.1, 0.5,
//                                      1, 1,
//                                      1, 1000, 100,
//                                      1, fs));
////
////     connections.push_back(Connection (violinStrings[3], violinStrings[4],
////                                       0.1, 0.5,
////                                       1, 1,
////                                       1, 10000, 100,
////                                       1, fs));
//
//    connections.push_back(Connection (violinStrings[numStrings-1], plates[0],
//                                      0.1, 0.3, 0.3,
//                                      1, 1,
//                                      1, 10000, 4000,
//                                      0.25, fs));

}

Instrument::~Instrument()
{
}

void Instrument::paint (Graphics& g)
{
    float dashPattern[2];
    dashPattern[0] = 3.0;
    dashPattern[1] = 5.0;
    
    g.setColour(Colours::orange);

    int stringIdx = 0;
    for (int i = 0; i < connections.size(); ++i)
    {
        if (connections[i].connectionType == stringString)
        {
            int cpX1 = connections[i].violinStrings[0]->getCP(connections[i].connID[0]);
            int cpX2 = connections[i].violinStrings[1]->getCP(connections[i].connID[1]);
            int y1 = connections[i].violinStrings[0]->getCy(connections[i].connID[0]);
            int y2 = connections[i].violinStrings[1]->getCy(connections[i].connID[1]);
            
            Line<float> connectionLine (ceil(cpX1 * getWidth() / connections[i].violinStrings[0]->getNumPoints()), stringIdx * (getHeight() / 2.0) / static_cast<double>(numStrings) + y1,
                                        ceil(cpX2 * getWidth() / connections[i].violinStrings[1]->getNumPoints()), (stringIdx + 1) * (getHeight() / 2.0) / static_cast<double>(numStrings) + y2);
            ++stringIdx;
            g.drawDashedLine(connectionLine, dashPattern, 2, dashPattern[0], 0);
        }
        else if (connections[i].connectionType == stringPlate)
        {
            int cpX1 = connections[i].violinStrings[0]->getCP(connections[i].connID[0]);
            int y1 = connections[i].violinStrings[0]->getCy(connections[i].connID[0]);
            auto [cpX2, cpY2] = connections[i].plates[0]->getCP(connections[i].connID[1]);
            
            double halfHeight = getHeight() / 2.0;
            int stateWidth = getWidth() / static_cast<double> (connections[i].plates[0]->getNumXPoints() - 4);
            int stateHeight = halfHeight / static_cast<double> (connections[i].plates[0]->getNumYPoints() - 4);
           
            Line<float> connectionLine (ceil(cpX1 * getWidth() / connections[i].violinStrings[0]->getNumPoints()), stringIdx * (halfHeight) / static_cast<double>(numStrings) + y1,
                                        (cpX2 - 1.5) * stateWidth, halfHeight + (cpY2 - 1.5) * stateHeight);
            g.drawDashedLine(connectionLine, dashPattern, 2, dashPattern[0], 0);
        }
    }
    g.setColour(Colour::greyLevel(0.5f).withAlpha(0.5f));
    for (int i = 1; i <= numStrings; ++i)
        g.drawLine(0, i * ((getHeight() / 2.0) / static_cast<double>(numStrings)), getWidth(), i * ((getHeight() / 2.0) / static_cast<double>(numStrings)));
}

void Instrument::resized()
{
    for (int i = 0; i < numStrings; ++i)
    {
        violinStrings[i]->setBounds(0 , i * ((getHeight() / 2.0) / static_cast<double>(numStrings)), getWidth(), (getHeight() / 2.0) / static_cast<double>(numStrings));
    }
    for (int i = 0; i < numPlates; ++i)
    {
        plates[i]->setBounds(0, getHeight() / 2.0 + i * (getHeight() / 2.0) / static_cast<double>(numPlates), getWidth(), (getHeight() / 2.0) / static_cast<double>(numPlates));
    }
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
        output[0] +=  violinStrings[i]->getOutput(0.75) * 600 * volume;
    }
    
//    output[0] += plates[0]->getOutput(0.3, 0.4) * 3;
    output[1] = output[0];
    
    //output[0] = violinStrings[2]->getOutput(0.75) * 600 + 0.1 * plates[0]->getOutput(0.3, 0.4) * 3;
    //output[1] = violinStrings[3]->getOutput(0.75) * 600 + 0.1 * plates[0]->getOutput(0.7, 0.4) * 3;
    
    return output;
}
