/*
  ==============================================================================

    VUMeter.h
    Created: 25 Jan 2019 4:25:59pm
    Author:  Silvin Willemsen

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class VUMeter    : public Component
{
public:
    VUMeter();
    ~VUMeter();

    void paint (Graphics&) override;
    void resized() override;

    void setLvlLeft (float lvl) { lvlLeft = lvl; };
    void setLvlRight (float lvl) { lvlRight = lvl; };
private:
    float lvlLeft = 0;
    float lvlRight = 0;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VUMeter)
};
