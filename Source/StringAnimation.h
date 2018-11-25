/*
  ==============================================================================

    StringAnimation.h
    Created: 23 Nov 2018 9:50:51am
    Author:  Nikolaj Schwab Andersson

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <vector>
using namespace std; 

class StringAnimation   : public Component,
                          private Timer
{
public:
    StringAnimation (int lengthInPixels, Colour stringColour, int height)
        : length (lengthInPixels), colour (stringColour), height(height)
    {
        // ignore mouse-clicks so that our parent can get them instead.
        setInterceptsMouseClicks (false, false);
        setSize (length, height);
        startTimerHz (60);

        states.resize(100);
        fill(states.begin(), states.end(), 0.0);
    }

    //==============================================================================
    void stringPlucked (float pluckPositionRelative)
    {
        amplitude = maxAmplitude * std::sin (pluckPositionRelative * MathConstants<float>::pi);
        phase = MathConstants<float>::pi;
    }

    //==============================================================================
    void paint (Graphics& g) override
    {
        g.setColour (colour);
        g.strokePath (generateStringPathAdvanced(), PathStrokeType (2.0f));
        //double y = states[conn1] * 50000 + height / 2.0;
        //g.drawEllipse(conn1, y, 10, 10, 1);
        g.setColour(Colours::orange);
        g.drawEllipse(floor(cx - 5), floor(cy - 5), 10, 10, 2);
        g.setColour (Colours::yellow);
        double opa = force / 100.0;
        if (opa >= 1.0)
        {
            g.setOpacity (1.0);
        } else {
            g.setOpacity(opa);
        }
        g.fillRect (floor(bowingPointX * getWidth()), floor(bowingPointY * height) - height / 2.0, 10, height);
    }
    
    Path generateStringPathAdvanced()
    {
        auto stringBounds = height/2.0;
        Path stringPath;
        stringPath.startNewSubPath (0, stringBounds);

        auto spacing = double(getWidth()) / double(states.size() + 1);
        auto x = spacing;
        
        for (int y = 0; y < states.size(); y++)
        {
            const float newY = states[y] * 50000 + stringBounds;
            stringPath.lineTo(x, newY);
            if (y == conn1)
            {
                cx = x;
                cy = newY;
            }
            x += spacing;
        }
        stringPath.lineTo(length, stringBounds);

        return stringPath;
    }
    
    
    
    void updateStringStates(vector<double> &newStates, double bpX, double bpY, double f, int conn)
    {
        if(isnan(newStates[5]))
            fill(states.begin(), states.end(), 0.0);
        else
            states = newStates;
        
        bowingPointX = bpX;
        bowingPointY = bpY;
        force = f;
        conn1 = conn;
    }
    //==============================================================================
    void timerCallback() override
    {
        repaint();
    }

private:
    //==============================================================================
    int length;
    Colour colour;

    vector<double> states;

    int height;
    float amplitude = 0.0f;
    const float maxAmplitude = 12.0f;
    float phase = 0.0f;
    
    double bowingPointX;
    double bowingPointY;
    double force;
    
    int cx = 0;
    int cy = 0;
    
    int conn1;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StringAnimation)
};
