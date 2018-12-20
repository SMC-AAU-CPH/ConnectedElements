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
    double baseFreq = 113.0;
    switch (instrumentType)
    {
        case twoStringViolin:
            frequencyInHz = {baseFreq,
                             baseFreq * pow(2, 7.0 / 12.0)};
            mix.resize (2, 0.5);
            break;
        case bowedSitar:
            frequencyInHz = {baseFreq,
                             baseFreq * pow(2, 7.0 / 12.0),
                
                             baseFreq * pow(2, 12.0 / 12.0),
                             baseFreq * pow(2, 14.0 / 12.0),
                             baseFreq * pow(2, 16.0 / 12.0),
                             baseFreq * pow(2, 17.0 / 12.0),
                             baseFreq * pow(2, 19.0 / 12.0),
                
                             baseFreq * pow(2, 12.0 / 12.0),
                             baseFreq * pow(2, 12.0 / 12.0),
                             baseFreq * pow(2, 11.0 / 12.0),
                             baseFreq * pow(2, 12.0 / 12.0),
                             baseFreq * pow(2, 14.0 / 12.0),
                             baseFreq * pow(2, 16.0 / 12.0),
                             baseFreq * pow(2, 17.0 / 12.0),
                             baseFreq * pow(2, 19.0 / 12.0),
                             baseFreq * pow(2, 21.0 / 12.0),
                             baseFreq * pow(2, 23.0 / 12.0),
                             baseFreq * pow(2, 24.0 / 12.0),
                             baseFreq * pow(2, 26.0 / 12.0),
                             baseFreq * pow(2, 28.0 / 12.0)
            };
            mix.resize (4, 0.5);
            break;
        case sitar:
            frequencyInHz = {
                
                baseFreq * pow(2, 12.0 / 12.0),
                baseFreq * pow(2, 14.0 / 12.0),
                baseFreq * pow(2, 16.0 / 12.0),
                baseFreq * pow(2, 17.0 / 12.0),
                baseFreq * pow(2, 19.0 / 12.0),
                
                baseFreq * pow(2, 12.0 / 12.0),
                baseFreq * pow(2, 12.0 / 12.0),
                baseFreq * pow(2, 11.0 / 12.0),
                baseFreq * pow(2, 12.0 / 12.0),
                baseFreq * pow(2, 14.0 / 12.0),
                baseFreq * pow(2, 16.0 / 12.0),
                baseFreq * pow(2, 17.0 / 12.0),
                baseFreq * pow(2, 19.0 / 12.0),
                baseFreq * pow(2, 21.0 / 12.0),
                baseFreq * pow(2, 23.0 / 12.0),
                baseFreq * pow(2, 24.0 / 12.0),
                baseFreq * pow(2, 26.0 / 12.0),
                baseFreq * pow(2, 28.0 / 12.0)
            };
             mix.resize (3, 0.5);
            break;
        case hurdyGurdy:
            frequencyInHz = {baseFreq * pow(2, -11.0 / 12.0),
                baseFreq * pow(2, -4.0 / 12.0),
                baseFreq,
                baseFreq * pow(2, 7.0 / 12.0),
                baseFreq * pow(2, 12.0 / 12.0),
                
                baseFreq * pow(2, 12.0 / 12.0),
                baseFreq * pow(2, 12.0 / 12.0),
                baseFreq * pow(2, 11.0 / 12.0),
                baseFreq * pow(2, 12.0 / 12.0),
                baseFreq * pow(2, 14.0 / 12.0),
                baseFreq * pow(2, 16.0 / 12.0),
                baseFreq * pow(2, 17.0 / 12.0),
                baseFreq * pow(2, 19.0 / 12.0),
                baseFreq * pow(2, 21.0 / 12.0),
                baseFreq * pow(2, 23.0 / 12.0),
                baseFreq * pow(2, 24.0 / 12.0),
                baseFreq * pow(2, 26.0 / 12.0),
                baseFreq * pow(2, 28.0 / 12.0)
            };
             mix.resize (4, 0.5);
            break;
        case dulcimer:
            frequencyInHz = {
                
                baseFreq * pow(2, 0.0 / 12.0),
                baseFreq * pow(2, 0.0 / 12.0),

                baseFreq * pow(2, 2.0 / 12.0),
                baseFreq * pow(2, 2.0 / 12.0),

                baseFreq * pow(2, 4.0 / 12.0),
                baseFreq * pow(2, 4.0 / 12.0),

                baseFreq * pow(2, 5.0 / 12.0),
                baseFreq * pow(2, 5.0 / 12.0),

                baseFreq * pow(2, 7.0 / 12.0),
                baseFreq * pow(2, 7.0 / 12.0),

                baseFreq * pow(2, 9.0 / 12.0),
                baseFreq * pow(2, 9.0 / 12.0),

                baseFreq * pow(2, 11.0 / 12.0),
                baseFreq * pow(2, 11.0 / 12.0),

                baseFreq * pow(2, 12.0 / 12.0),
                baseFreq * pow(2, 12.0 / 12.0),

                baseFreq * pow(2, 14.0 / 12.0),
                baseFreq * pow(2, 14.0 / 12.0),

                baseFreq * pow(2, 16.0 / 12.0),
                baseFreq * pow(2, 16.0 / 12.0),

                baseFreq * pow(2, 17.0 / 12.0),
                baseFreq * pow(2, 17.0 / 12.0),

                baseFreq * pow(2, 19.0 / 12.0),
                baseFreq * pow(2, 19.0 / 12.0),

                baseFreq * pow(2, 21.0 / 12.0),
                baseFreq * pow(2, 21.0 / 12.0),

                baseFreq * pow(2, 23.0 / 12.0),
                baseFreq * pow(2, 23.0 / 12.0),

                baseFreq * pow(2, 24.0 / 12.0),
                baseFreq * pow(2, 24.0 / 12.0),
                
                baseFreq * pow(2, 26.0 / 12.0),
                baseFreq * pow(2, 26.0 / 12.0),

                baseFreq * pow(2, 28.0 / 12.0),
                baseFreq * pow(2, 28.0 / 12.0),
                
                baseFreq * pow(2, 29.0 / 12.0),
                baseFreq * pow(2, 29.0 / 12.0),
                
                baseFreq * pow(2, 31.0 / 12.0),
                baseFreq * pow(2, 31.0 / 12.0),

                baseFreq * pow(2, 33.0 / 12.0),
                baseFreq * pow(2, 33.0 / 12.0),

                baseFreq * pow(2, 35.0 / 12.0),
                baseFreq * pow(2, 35.0 / 12.0),

                baseFreq * pow(2, 36.0 / 12.0),
                baseFreq * pow(2, 36.0 / 12.0),

            };
            // slightly detune
            for (int i = 0; i < frequencyInHz.size(); i = i + 2)
            {
                if (r.nextFloat() > 0.5)
                    frequencyInHz[i] += frequencyInHz[i]*0.01 * r.nextFloat();
                else 
                    frequencyInHz[i] -= frequencyInHz[i]*0.01 * r.nextFloat();
            }

            mix.resize (2, 0.5);
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
                violinStrings.add (new ViolinString(frequencyInHz[i], fs, objectTypes[i], i, instrumentType));
                addAndMakeVisible (violinStrings[i]);
                ++numBowedStrings;
                break;
            }
            case pluckedString:
            {
                if (i > frequencyInHz.size() - 1)
                    frequencyInHz.push_back(frequencyInHz[frequencyInHz.size()-1]);
                violinStrings.add (new ViolinString(frequencyInHz[i], fs, objectTypes[i], i, instrumentType));
                addAndMakeVisible (violinStrings[i]);
                ++numPluckedStrings;
                break;
            }
            case sympString:
            {
                if (i > frequencyInHz.size() - 1)
                    frequencyInHz.push_back(frequencyInHz[frequencyInHz.size()-1]);
                violinStrings.add (new ViolinString(frequencyInHz[i], fs, objectTypes[i], i, instrumentType));
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
        for (int i = 0; i < numBowedStrings; ++i)
        {
            connections.push_back(Connection (violinStrings[i], plates[0],
                                            (violinStrings[i]->getNumPoints() - 5) / static_cast<double>(violinStrings[i]->getNumPoints()),
                                              0.4 + i * 0.05, 0.3,
                                              1, 1,
                                              0, 10000, 1,
                                              800, fs));
        }
        int numUnbowedStrings = numPluckedStrings + numSympStrings;
        switch (instrumentType)
        {
            case bowedSitar:
            case sitar:
            case hurdyGurdy:
                for (int i = numBowedStrings; i < getTotNumStrings(); ++i)
                {
                    double halfFlag = false;
                    if ((i - numBowedStrings > (numUnbowedStrings / 2)) && !halfFlag)
                        halfFlag = true;
                    
                    double x = (i - (halfFlag ? numUnbowedStrings / 2 : 0)) / static_cast<double>(plates[0]->getNumXPoints());
                    double y = (halfFlag ? 0.5 : 0.6);
                    connections.push_back(Connection (violinStrings[i], plates[0],
                                                      (violinStrings[i]->getNumPoints() - 3) / static_cast<double>(violinStrings[i]->getNumPoints()),
                                                      x,
                                                      y,
                                                      1, 1,
                                                      1, 10000, 1,
                                                      violinStrings[i]->getStringType() == pluckedString ? 800 : 0.25, fs));
                }
                break;
            case dulcimer:
                int j = 0;
                for (int i = 1; i < numPluckedStrings; i = i + 2)
                {
//                    connections.push_back (Connection (violinStrings[i], violinStrings[i+1],
//                                                      4 / static_cast<double>(violinStrings[i]->getNumPoints()),
//                                                      4 / static_cast<double>(violinStrings[i+1]->getNumPoints()),
//                                                      1, 1,
//                                                      1, 10000, 1,
//                                                      1, fs));
//
                    double halfFlag = false;
                    if ((j > (numPluckedStrings / 4)) && !halfFlag)
                        halfFlag = true;
                    
                    double x = (j - (halfFlag ? numPluckedStrings / 4 : 0)) / static_cast<double>(plates[0]->getNumXPoints());
                    double y = (halfFlag ? 0.5 : 0.6);
                    
                    connections.push_back (Connection (violinStrings[i], plates[0],
                                                      (violinStrings[i]->getNumPoints() - 4) / static_cast<double>(violinStrings[i]->getNumPoints()),
                                                      x + 0.2,
                                                      y,
                                                      1, 1,
                                                      1, 10000, 1,
                                                      1, fs));
                    ++j;
                }
                break;
        }
    }
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
            case bowedStringPluckedString:
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
            case pluckedStringSympString:
            case pluckedStringPluckedString:
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
                Line<float> connectionLine (ceil(cpX1 * getWidth() / string1->getNumPoints()),
                                            (sympStringID1 * totSympStringHeight) / static_cast<double>(numSympStrings + numPluckedStrings) + y1,
                                            ceil(cpX2 * getWidth() / string2->getNumPoints()),
                                            (sympStringID2 * totSympStringHeight) / static_cast<double>(numSympStrings + numPluckedStrings) + y2);
                g.drawDashedLine (connectionLine, dashPattern, 2, dashPattern[0], 0);
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
        // plucked strings
        for (int i = numBowedStrings; i < numBowedStrings + numPluckedStrings; ++i)
        {
            switch (instrumentType)
            {
                case bowedSitar:
                case hurdyGurdy:
                    mixVal = mix[1];
                    break;
                case sitar:
                case dulcimer:
                    mixVal = mix[0];
                    break;
            }
            output[0] +=  violinStrings[i]->getOutput(0.75) * 800 * mixVal;
        }
        // sympathetic strings
        for (int i = numBowedStrings + numPluckedStrings; i < numBowedStrings + numPluckedStrings + numSympStrings; i++)
        {
            switch (instrumentType)
            {
                case bowedSitar:
                case hurdyGurdy:
                    mixVal = mix[2];
                    break;
                case sitar:
                    mixVal = mix[1];
                case dulcimer:
                    mixVal = 0;
                    break;
            }
            output[0] +=  violinStrings[i]->getOutput(0.68) * 2000 * mixVal;
        }
        for (int i = 0; i < numPlates; i++)
        {
            switch (instrumentType)
            {
                case bowedSitar:
                case hurdyGurdy:
                    mixVal = mix[3];
                    break;
                case sitar:
                    mixVal = mix[2];
                case dulcimer:
                    mixVal = mix[1];
                    break;
            }
            output[0] += plates[0]->getOutput(0.8, 0.4) * 30 * mixVal;
        }
    }
    output[1] = output[0];
    
    return output;
}
