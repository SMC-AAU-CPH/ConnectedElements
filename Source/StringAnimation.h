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
        g.setColour (Colours::yellow);
        g.setOpacity (0.5);
        g.fillEllipse (floor(bowingPointX * getWidth()), floor(bowingPointY * height) - height / 2.0, 5 + floor(force / 2.0), height);
    }

/*    Path generateStringPathSimple() const
    {
        // this determines the decay of the visible string vibration.
        amplitude *= 0.99f;
        // this determines the visible vibration frequency.
        // just an arbitrary number chosen to look OK:
        auto phaseStep = 400.0f / length;
        
        phase += phaseStep;
        
        if (phase >= MathConstants<float>::twoPi)
            phase -= MathConstants<float>::twoPi;
        
        auto y = height / 2.0f;

        Path stringPath;
        stringPath.startNewSubPath (0, y);
        stringPath.quadraticTo (length / 2.0f, y + (std::sin (phase) * amplitude), (float) length, y);
        return stringPath;
    }
*/
    Path generateStringPathAdvanced() const
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
            x += spacing;
        }
        stringPath.lineTo(length, stringBounds);

        return stringPath;
    }
    
    void updateStringStates(vector<double> &newStates, double bpX, double bpY, double f)
    {
        if(isnan(newStates[5]))
            fill(states.begin(), states.end(), 0.0);
        else
            states = newStates;
        
        bowingPointX = bpX;
        bowingPointY = bpY;
        force = f;
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
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StringAnimation)
};
