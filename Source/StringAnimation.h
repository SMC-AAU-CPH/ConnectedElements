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
    StringAnimation (int lengthInPixels, Colour stringColour)
        : length (lengthInPixels), colour (stringColour)
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
    }

    Path generateStringPathSimple() const
    {
        auto y = height / 2.0f;

        Path stringPath;
        stringPath.startNewSubPath (0, y);
        stringPath.quadraticTo (length / 2.0f, y + (std::sin (phase) * amplitude), (float) length, y);
        return stringPath;
    }

    Path generateStringPathAdvanced() const
    {
     
        //auto h = height;
        auto stringYstart = height/2;
        Path stringPath;
        stringPath.startNewSubPath (0, stringYstart);

        auto spacing = double(length) / double(states.size());
        auto x = spacing * 800 + 20;
        
        for (int y = 0; y < states.size(); y++)
        {
            
            const float newY = height/2;//states[y];
            stringPath.lineTo(x, newY);
            x += spacing;
        }
        //stringPath.lineTo(length, height/2);
        stringPath.closeSubPath();
        //stringPath.quadraticTo (length / 2.0f, y + (std::sin (phase) * amplitude), (float) length, y);
        return stringPath;
    }
    void updateStringStates(vector<double> &newStates)
    {

    }
    //==============================================================================
    void timerCallback() override
    {
        updateAmplitude();
        updatePhase();
        repaint();
    }

    void updateAmplitude()
    {
        // this determines the decay of the visible string vibration.
        amplitude *= 0.99f;
    }

    void updatePhase()
    {
        // this determines the visible vibration frequency.
        // just an arbitrary number chosen to look OK:
        auto phaseStep = 400.0f / length;

        phase += phaseStep;

        if (phase >= MathConstants<float>::twoPi)
            phase -= MathConstants<float>::twoPi;
    }

private:
    //==============================================================================
    int length;
    Colour colour;

    vector<double> states;

    int height = 20;
    float amplitude = 0.0f;
    const float maxAmplitude = 12.0f;
    float phase = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StringAnimation)
};
