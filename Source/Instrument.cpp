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
Instrument::Instrument (InstrumentType instrumentType, vector<ObjectType> objectTypes, double fs, int stringPlateDivision, int bowedSympDivision)
: fs (fs), stringPlateDivision (stringPlateDivision), instrumentType (instrumentType), bowedSympDivision (bowedSympDivision)
{
    
    
    vector<double> frequencyInHz;
    
    switch (instrumentType)
    {
        case twoStringViolin:
            frequencyInHz = {110.0,
                             110.0 * pow(2, 7.0 / 12.0)};
            mix.resize (2, 0.5);
            break;
        case bowedSitar:
            frequencyInHz = {110.0,
                             110.0 * pow(2, 7.0 / 12.0),
                
                             110.0 * pow(2, 12.0 / 12.0),
                             110.0 * pow(2, 14.0 / 12.0),
                             110.0 * pow(2, 16.0 / 12.0),
                             110.0 * pow(2, 17.0 / 12.0),
                             110.0 * pow(2, 19.0 / 12.0),
                
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
            mix.resize (4, 0.5);
            break;
        case sitar:
            frequencyInHz = {
                
                110.0 * pow(2, 12.0 / 12.0),
                110.0 * pow(2, 14.0 / 12.0),
                110.0 * pow(2, 16.0 / 12.0),
                110.0 * pow(2, 17.0 / 12.0),
                110.0 * pow(2, 19.0 / 12.0),
                
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
             mix.resize (4, 0.5);
            break;
        case hurdyGurdy:

             mix.resize (4, 0.5);
            break;
        case dulcimer:


            frequencyInHz = {
                
                110.0 * pow(2, 12.0 / 12.0),
                110.0 * pow(2, 12.0 / 12.0),

                110.0 * pow(2, 14.0 / 12.0),
                110.0 * pow(2, 14.0 / 12.0),

                110.0 * pow(2, 16.0 / 12.0),
                110.0 * pow(2, 16.0 / 12.0),

                110.0 * pow(2, 17.0 / 12.0),
                110.0 * pow(2, 17.0 / 12.0),

                110.0 * pow(2, 19.0 / 12.0),
                110.0 * pow(2, 19.0 / 12.0),

                110.0 * pow(2, 21.0 / 12.0),
                110.0 * pow(2, 21.0 / 12.0),

                110.0 * pow(2, 23.0 / 12.0),
                110.0 * pow(2, 23.0 / 12.0),

                110.0 * pow(2, 24.0 / 12.0),
                110.0 * pow(2, 24.0 / 12.0),
                
                110.0 * pow(2, 26.0 / 12.0),
                110.0 * pow(2, 26.0 / 12.0),

                110.0 * pow(2, 28.0 / 12.0),
                110.0 * pow(2, 28.0 / 12.0),
                
                110.0 * pow(2, 29.0 / 12.0),
                110.0 * pow(2, 29.0 / 12.0),
                
                110.0 * pow(2, 31.0 / 12.0),
                110.0 * pow(2, 31.0 / 12.0)
            };
            // slightly detune
            for (int i = 0; i < frequencyInHz.size(); i++)
            {
                if (r.nextFloat() > 0.5)
                    frequencyInHz[i] += r.nextFloat() * i;
                else 
                    frequencyInHz[i] -= r.nextFloat() * i;
            }

             mix.resize (4, 0.5);
            break;
    }
    
    for (int i = 0; i < objectTypes.size(); ++i)
    {
        switch(objectTypes[i])
        {
            case bowedString:
            {
                if (i > frequencyInHz.size() - 1)
                    frequencyInHz.push_back(frequencyInHz[frequencyInHz.size()-1]);
                violinStrings.add (new ViolinString(frequencyInHz[i], fs, objectTypes[i], i));
                addAndMakeVisible (violinStrings[i]);
                ++numBowedStrings;
                break;
            }
            case pluckedString:
            {
                if (i > frequencyInHz.size() - 1)
                    frequencyInHz.push_back(frequencyInHz[frequencyInHz.size()-1]);
                violinStrings.add (new ViolinString(frequencyInHz[i], fs, objectTypes[i], i));
                addAndMakeVisible (violinStrings[i]);
                ++numPluckedStrings;
                break;
            }
            case sympString:
            {
                if (i > frequencyInHz.size() - 1)
                    frequencyInHz.push_back(frequencyInHz[frequencyInHz.size()-1]);
                violinStrings.add (new ViolinString(frequencyInHz[i], fs, objectTypes[i], i));
                addAndMakeVisible (violinStrings[i]);
                ++numSympStrings;
                break;
            }
            case plate:
            {
                plates.add (new Plate(fs));
                addAndMakeVisible (plates[0]);
                ++numPlates;
                break;
            }
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
                                              0, 10000, 1,
                                              800, fs));
        }
        int numUnbowedStrings = numPluckedStrings + numSympStrings;
        for (int i = numBowedStrings; i < numBowedStrings + numPluckedStrings; ++i)
        {
            double halfFlag = false;
            if ((i - numBowedStrings > (numUnbowedStrings / 2)) && !halfFlag)
                halfFlag = true;
                                    
            double x = (i - (halfFlag ? numUnbowedStrings / 2 : 0)) / static_cast<double>(plates[0]->getNumXPoints());
            double y = (halfFlag ? 0.5 : 0.4);
            connections.push_back(Connection (violinStrings[i], plates[0],
                                              (violinStrings[i]->getNumPoints() - 10) / static_cast<double>(violinStrings[i]->getNumPoints()),
                                              x + 0.1,
                                              y,
                                              1, 1,
                                              1, 10000, 1,
                                              800, fs));
//        }
        
//        for (int i = numBowedStrings + numPluckedStrings; i < getTotNumStrings(); ++i)
//        {
//            double halfFlag = false;
//            if ((i - numBowedStrings > (numUnbowedStrings / 2)) && !halfFlag)
//                halfFlag = true;
//
//            double x = (i - (halfFlag ? numUnbowedStrings / 2 : 0)) / static_cast<double>(plates[0]->getNumXPoints());
//            double y = (halfFlag ? 0.5 : 0.4);
//            connections.push_back(Connection (violinStrings[i], plates[0],
//                                              (violinStrings[i]->getNumPoints() - 5) / static_cast<double>(violinStrings[i]->getNumPoints()),
//                                              x + 0.1,
//                                              y,
//                                              1, 1,
//                                              1, 10000, 1,
//                                              0.25, fs));
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
        int numUnbowedStrings = numSympStrings + numPluckedStrings;
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
                                            ceil((cpX2 * (getWidth() - bowedSympDivision)) / string2->getNumPoints()) + bowedSympDivision, ((numPluckedStrings + sympStringID) * totSympStringHeight) / static_cast<double>(numSympStrings) + y2);
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
                Line<float> connectionLine (ceil(cpX1 * getWidth() / string1->getNumPoints()),totBowedStringHeight +  (numPluckedStrings + sympStringID1) * totSympStringHeight / static_cast<double>(numSympStrings) + y1,
                                            ceil(cpX2 * getWidth() / string2->getNumPoints()), totBowedStringHeight + (numPluckedStrings + sympStringID2) * totSympStringHeight / static_cast<double>(numSympStrings) + y2);
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
            case pluckedStringPlate:
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
                
                Line<float> connectionLine (ceil(cpX1 * (getWidth() - bowedSympDivision) / string1->getNumPoints()) + bowedSympDivision, sympStringID * totSympStringHeight / static_cast<double>(numUnbowedStrings) + y1,
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
    int numUnbowedStrings = numPluckedStrings + numSympStrings;
    for (int i = 0; i < numUnbowedStrings; ++i)
    {
        violinStrings[i+numBowedStrings]->setBounds(bowedSympDivision , i * (totSympStringHeight / static_cast<double>(numUnbowedStrings)), getWidth() - bowedSympDivision, stringPlateDivision / static_cast<double>(numUnbowedStrings));
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
            case bowedStringPluckedString:
            case bowedStringSympString:
            case pluckedStringPluckedString:
            case pluckedStringSympString:
            case sympStringSympString:
                connections[c].violinStrings[0]->addJFc(connections[c].getJFc()[0], connections[c].violinStrings[0]->getCP(connections[c].connID[0]));
                connections[c].violinStrings[1]->addJFc(connections[c].getJFc()[1], connections[c].violinStrings[1]->getCP(connections[c].connID[1]));
                break;
            case bowedStringPlate:
            case pluckedStringPlate:
            case sympStringPlate:
                connections[c].violinStrings[0]->addJFc(connections[c].getJFc()[0], connections[c].violinStrings[0]->getCP(connections[c].connID[0]));
                connections[c].plates[0]->addJFc (connections[c].getJFc()[1], connections[c].plates[0]->getCP(connections[c].connID[1]));
                break;
            case platePlate:
            {
                connections[c].plates[0]->addJFc(connections[c].getJFc()[0], connections[c].plates[0]->getCP(connections[c].connID[0]));
                connections[c].plates[1]->addJFc (connections[c].getJFc()[1], connections[c].plates[1]->getCP(connections[c].connID[1]));
                break;
            }
        }
    }
    
    for (auto violinString : violinStrings)
        violinString->updateUVectors();
    
    for (auto plate : plates)
        plate->updateUVectors();
    
    
    for (int i = 0; i < numBowedStrings; i++)
    {
        output[0] +=  violinStrings[i]->getOutput(0.75) * 800 * (instrumentType == twoStringViolin ? mix[i] : mix[0]);
    }
    if (instrumentType != twoStringViolin)
    {
        for (int i = numBowedStrings; i < numBowedStrings + numPluckedStrings; ++i)
        {
            output[0] +=  violinStrings[i]->getOutput(0.75) * 800 * mix[1];
        }
        for (int i = numBowedStrings + numPluckedStrings; i < numBowedStrings + numPluckedStrings + numSympStrings; i++)
        {
            output[0] +=  violinStrings[i]->getOutput(0.75) * 2000 * mix[2];
        }
        for (int i = 0; i < numPlates; i++)
        {
            output[0] += plates[0]->getOutput(0.5, 0.4) * 30 * mix[3];
        }
    }
    output[1] = output[0];
    
    //output[0] = violinStrings[2]->getOutput(0.75) * 600 + 0.1 * plates[0]->getOutput(0.3, 0.4) * 3;
    //output[1] = violinStrings[3]->getOutput(0.75) * 600 + 0.1 * plates[0]->getOutput(0.7, 0.4) * 3;
    
    return output;
}
