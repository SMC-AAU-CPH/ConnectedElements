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
Instrument::Instrument (vector<ObjectType> objectTypes, double fs, int stringPlateDivision, int bowedSympDivision) : fs (fs),
                                                                                              stringPlateDivision (stringPlateDivision),
                                                                                              bowedSympDivision (bowedSympDivision)
{
    
    vector<double> frequencyInHz = {110.0,
                                    110.0 * pow(2, 7.0 / 12.0),
        
                                    110.0 * pow(2, 12.0 / 12.0),
                                    110.0 * pow(2, 14.0 / 12.0),
                                    110.0 * pow(2, 16.0 / 12.0),
                                    110.0 * pow(2, 17.0 / 12.0),
                                    110.0 * pow(2, 19.0 / 12.0),
                                    110.0 * pow(2, 21.0 / 12.0),
                                    110.0 * pow(2, 23.0 / 12.0),
                                    110.0 * pow(2, 24.0 / 12.0),
        
                                    110.0 * pow(2, 12.0 / 12.0),
                                    110.0 * pow(2, 12.0 / 12.0),
                                    110.0 * pow(2, 11.0 / 12.0),
                                    110.0 * pow(2, 12.0 / 12.0),
                                    110.0 * pow(2, 14.0 / 12.0),
                                    110.0 * pow(2, 16.0 / 12.0),
                                    110.0 * pow(2, 17.0 / 12.0),
                                    110.0 * pow(2, 19.0 / 12.0),
                                    110.0 * pow(2, 21.0 / 12.0),
                                    110.0 * pow(2, 23.0 / 12.0),
                                    110.0 * pow(2, 24.0 / 12.0),
                                    110.0 * pow(2, 26.0 / 12.0),
                                    110.0 * pow(2, 28.0 / 12.0)
    };
    
    for (int i = 0; i < objectTypes.size(); ++i)
    {
        if (objectTypes[i] == bowedString)
        {
            if (i > frequencyInHz.size() - 1)
                frequencyInHz.push_back(frequencyInHz[frequencyInHz.size()-1]);
            violinStrings.add (new ViolinString(frequencyInHz[i], fs, objectTypes[i], i));
            addAndMakeVisible (violinStrings[i]);
            ++numBowedStrings;
        }
        else if (objectTypes[i] == sympString)
        {
            if (i > frequencyInHz.size() - 1)
                frequencyInHz.push_back(frequencyInHz[frequencyInHz.size()-1]);
            
            violinStrings.add (new ViolinString(frequencyInHz[i], fs, objectTypes[i], i));
            addAndMakeVisible (violinStrings[i]);
            ++numSympStrings;
        }
        else if (objectTypes[i] == plate)
        {
            plates.add (new Plate(fs));
            addAndMakeVisible (plates[0]);
            ++numPlates;
        }
    }

    if (numPlates != 0)
    {
//        connections.push_back(Connection (violinStrings[0], plates[0],
//                                          (violinStrings[0]->getNumPoints() - 5) / static_cast<double>(violinStrings[0]->getNumPoints()),
//                                          (6) / static_cast<double>(plates[0]->getNumXPoints()), 0.4,
//                                          1, 1,
//                                          1, 10000, 1,
//                                          violinStrings[0]->getStringType() == bowedString ? 1 : 1, fs));
//        connections.push_back(Connection (violinStrings[1], plates[0],
//                                          (violinStrings[1]->getNumPoints() - 5) / static_cast<double>(violinStrings[1]->getNumPoints()),
//                                          (5) / static_cast<double>(plates[0]->getNumXPoints()), 0.4,
//                                          1, 1,
//                                          1, 10000, 1,
//                                          violinStrings[1]->getStringType() == bowedString ? 1 : 1, fs));
        for (int i = 0; i < numBowedStrings; ++i)
        {
            connections.push_back(Connection (violinStrings[i], plates[0],
                                              (violinStrings[i]->getNumPoints() - 5) / static_cast<double>(violinStrings[i]->getNumPoints()),
                                              0.5, (0.3 + i * 0.3),
                                              1, 1,
                                              1, 10000, 1,
                                              violinStrings[i]->getStringType() == bowedString ? 1 : 1, fs));
        }
        int j = 2;
        for (int i = 2; i < numBowedStrings + numSympStrings; ++i)
        {
            
            double x = (j + 2 - (j < (numSympStrings / 2) ? 0 : 8)) / static_cast<double>(plates[0]->getNumXPoints());
            double y = (j < (numSympStrings / 2) ? 0.4 : 0.5);
            connections.push_back(Connection (violinStrings[i], plates[0],
                                              (violinStrings[i]->getNumPoints() - 5) / static_cast<double>(violinStrings[i]->getNumPoints()),
                                              x,
                                              y,
                                              1, 1,
                                              1, 10000, 1,
                                              violinStrings[i]->getStringType() == bowedString ? 1 : 1, fs));
            ++j;
        }
    }
//    connections.push_back(Connection (violinStrings[0], violinStrings[1],
//                                      0.5, 0.5,
//                                      1, 1,
//                                      1, 1000, 100,
//                                      1, fs));
////
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
//
//     connections.push_back(Connection (violinStrings[1], violinStrings[3],
//                                       0.1, 0.5,
//                                       1, 1,
//                                       1, 10000, 100,
//                                       1, fs));
//
//    connections.push_back(Connection (violinStrings[numStrings-1], plates[0],
//                                      0.1, 0.3, 0.3,
//                                      1, 1,
//                                      1, 10000, 4000,
//                                      0.25, fs));
//    totBowedStringHeight = stringPlateDivision - (numSympStrings * sympStringHeight);
//    totSympStringHeight = sympStringHeight * numSympStrings;
    totBowedStringHeight = stringPlateDivision;
    totSympStringHeight = stringPlateDivision;
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

    for (int i = 0; i < connections.size(); ++i)
    {
        switch(connections[i].connectionType)
        {
            case bowedStringBowedString:
            {
                ViolinString* string1 = connections[i].violinStrings[0];
                ViolinString* string2 = connections[i].violinStrings[1];
                int cpX1 = string1->getCP(connections[i].connID[0]);
                int cpX2 = string2->getCP(connections[i].connID[1]);
                int y1 = string1->getCy(connections[i].connID[0]);
                int y2 = string2->getCy(connections[i].connID[1]);
                
                Line<float> connectionLine (ceil(cpX1 * bowedSympDivision / string1->getNumPoints()), string1->getStringID() * totBowedStringHeight / static_cast<double>(numBowedStrings) + y1,
                                            ceil(cpX2 * bowedSympDivision / string2->getNumPoints()), string2->getStringID() * totBowedStringHeight / static_cast<double>(numBowedStrings) + y2);
                g.drawDashedLine(connectionLine, dashPattern, 2, dashPattern[0], 0);
                break;
            }
            case bowedStringSympString:
            {
                ViolinString* string1 = connections[i].violinStrings[0];
                ViolinString* string2 = connections[i].violinStrings[1];
                int cpX1 = string1->getCP(connections[i].connID[0]);
                int cpX2 = string2->getCP(connections[i].connID[1]);
                int y1 = string1->getCy(connections[i].connID[0]);
                int y2 = string2->getCy(connections[i].connID[1]);
                
                int sympStringID = string2->getStringID() - numBowedStrings;
                Line<float> connectionLine (ceil((cpX1 * bowedSympDivision) / string1->getNumPoints()), string1->getStringID() * totBowedStringHeight / static_cast<double>(numBowedStrings) + y1,
                                            ceil((cpX2 * (getWidth() - bowedSympDivision)) / string2->getNumPoints()) + bowedSympDivision, sympStringID * totSympStringHeight / static_cast<double>(numSympStrings) + y2);
                g.drawDashedLine(connectionLine, dashPattern, 2, dashPattern[0], 0);
                break;
            }
            case sympStringSympString:
            {
                ViolinString* string1 = connections[i].violinStrings[0];
                ViolinString* string2 = connections[i].violinStrings[1];
                int cpX1 = string1->getCP(connections[i].connID[0]);
                int cpX2 = string2->getCP(connections[i].connID[1]);
                int y1 = string1->getCy(connections[i].connID[0]);
                int y2 = string2->getCy(connections[i].connID[1]);
                
                int sympStringID1 = string1->getStringID() - numBowedStrings;
                int sympStringID2 = string2->getStringID() - numBowedStrings;
                Line<float> connectionLine (ceil(cpX1 * getWidth() / string1->getNumPoints()),totBowedStringHeight +  sympStringID1 * totSympStringHeight / static_cast<double>(numSympStrings) + y1,
                                            ceil(cpX2 * getWidth() / string2->getNumPoints()), totBowedStringHeight + sympStringID2 * totSympStringHeight / static_cast<double>(numSympStrings) + y2);
                g.drawDashedLine(connectionLine, dashPattern, 2, dashPattern[0], 0);
                break;
            }
            case bowedStringPlate:
            {
                ViolinString* string1 = connections[i].violinStrings[0];
                Plate* plate1 = connections[i].plates[0];
                int cpX1 = string1->getCP(connections[i].connID[0]);
                int y1 = string1->getCy(connections[i].connID[0]);
                auto [cpX2, cpY2] = plate1->getCP(connections[i].connID[1]);
                
                int stateWidth = getWidth() / static_cast<double> (plate1->getNumXPoints() - 4);
                int stateHeight = (getHeight() - stringPlateDivision) / static_cast<double> (plate1->getNumYPoints() - 4);
                
                Line<float> connectionLine (ceil((cpX1 * bowedSympDivision) / string1->getNumPoints()), string1->getStringID() * totBowedStringHeight / static_cast<double>(numBowedStrings) + y1,
                                            (cpX2 - 1.5) * stateWidth, stringPlateDivision + (cpY2 - 1.5) * stateHeight);
                g.drawDashedLine(connectionLine, dashPattern, 2, dashPattern[0], 0);
                break;
            }
            case sympStringPlate:
            {
                ViolinString* string1 = connections[i].violinStrings[0];
                Plate* plate1 = connections[i].plates[0];
                int cpX1 = string1->getCP(connections[i].connID[0]);
                int y1 = string1->getCy(connections[i].connID[0]);
                auto [cpX2, cpY2] = plate1->getCP(connections[i].connID[1]);
                
                int stateWidth = getWidth() / static_cast<double> (plate1->getNumXPoints() - 4);
                int stateHeight = (getHeight() - stringPlateDivision) / static_cast<double> (plate1->getNumYPoints() - 4);
                
                int sympStringID = string1->getStringID() - numBowedStrings;
                
                Line<float> connectionLine (ceil(cpX1 * (getWidth() - bowedSympDivision) / string1->getNumPoints()) + bowedSympDivision, sympStringID * totSympStringHeight / static_cast<double>(numSympStrings) + y1,
                                            (cpX2 - 1.5) * stateWidth, stringPlateDivision + (cpY2 - 1.5) * stateHeight);
                g.drawDashedLine(connectionLine, dashPattern, 2, dashPattern[0], 0);
                break;
            }
            case platePlate:
                break;
        }
    }
    g.setColour(Colour::greyLevel(0.5f).withAlpha(0.5f));
    g.drawLine(bowedSympDivision, 0, bowedSympDivision, stringPlateDivision);
}

void Instrument::resized()
{
    for (int i = 0; i < numBowedStrings; ++i)
    {
        violinStrings[i]->setBounds(0 , i * (totBowedStringHeight / static_cast<double>(numBowedStrings)), bowedSympDivision, stringPlateDivision / static_cast<double>(numBowedStrings));
    }
    for (int i = 0; i < numSympStrings; ++i)
    {
        violinStrings[i+numBowedStrings]->setBounds(bowedSympDivision , i * (totSympStringHeight / static_cast<double>(numSympStrings)), getWidth() - bowedSympDivision, stringPlateDivision / static_cast<double>(numSympStrings));
    }
    for (int i = 0; i < numPlates; ++i)
    {
        plates[i]->setBounds(0, stringPlateDivision + i * stringPlateDivision / static_cast<double>(numPlates), getWidth(), (getHeight() - stringPlateDivision) / static_cast<double>(numPlates));
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
        switch (connections[c].connectionType)
        {
            case bowedStringBowedString:
            case bowedStringSympString:
            case sympStringSympString:
                connections[c].violinStrings[0]->addJFc(connections[c].getJFc()[0], connections[c].violinStrings[0]->getCP(connections[c].connID[0]));
                connections[c].violinStrings[1]->addJFc(connections[c].getJFc()[1], connections[c].violinStrings[1]->getCP(connections[c].connID[1]));
                break;
            case bowedStringPlate:
            case sympStringPlate:
                connections[c].violinStrings[0]->addJFc(connections[c].getJFc()[0], connections[c].violinStrings[0]->getCP(connections[c].connID[0]));
                connections[c].plates[0]->addJFc (connections[c].getJFc()[1], connections[c].plates[0]->getCP(connections[c].connID[1]));
                break;
            case platePlate:
                connections[c].plates[0]->addJFc(connections[c].getJFc()[0], connections[c].plates[0]->getCP(connections[c].connID[0]));
                connections[c].plates[1]->addJFc (connections[c].getJFc()[1], connections[c].plates[1]->getCP(connections[c].connID[1]));
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
        output[0] +=  violinStrings[i]->getOutput(0.75) * 800 * volume;
    }
    
//    output[0] += plates[0]->getOutput(0.5, 0.4) * 3;
    output[1] = output[0];
    
    //output[0] = violinStrings[2]->getOutput(0.75) * 600 + 0.1 * plates[0]->getOutput(0.3, 0.4) * 3;
    //output[1] = violinStrings[3]->getOutput(0.75) * 600 + 0.1 * plates[0]->getOutput(0.7, 0.4) * 3;
    
    return output;
}
