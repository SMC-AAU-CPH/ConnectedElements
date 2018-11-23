/*
  ==============================================================================

    StringAnimation.h
    Created: 23 Nov 2018 9:50:51am
    Author:  Nikolaj Schwab Andersson

  ==============================================================================
*/

#pragma once
//==============================================================================
/*
    This component represents a horizontal vibrating musical string of fixed height
    and variable length. The string can be excited by calling stringPlucked().
*/
#include "../JuceLibraryCode/JuceHeader.h"

class StringComponent   : public Component,
                          private Timer
{
public:
    StringComponent (int lengthInPixels, Colour stringColour)
        : length (lengthInPixels), colour (stringColour)
    {
        // ignore mouse-clicks so that our parent can get them instead.
        setInterceptsMouseClicks (false, false);
        setSize (length, height);
        startTimerHz (60);
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
        g.strokePath (generateStringPath(), PathStrokeType (2.0f));
    }

    Path generateStringPath() const
    {
        auto y = height / 2.0f;

        Path stringPath;
        stringPath.startNewSubPath (0, y);
        stringPath.quadraticTo (length / 2.0f, y + (std::sin (phase) * amplitude), (float) length, y);
        return stringPath;
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

    int height = 20;
    float amplitude = 0.0f;
    const float maxAmplitude = 12.0f;
    float phase = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StringComponent)
};