/*
  ==============================================================================

    VUMeter.cpp
    Created: 25 Jan 2019 4:25:59pm
    Author:  Silvin Willemsen

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "VUMeter.h"

//==============================================================================
VUMeter::VUMeter()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

}

VUMeter::~VUMeter()
{
}

void VUMeter::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */
    ColourGradient gradient (Colours::red, 0, 0,
                             Colours::green, 0, getHeight(),
                             false);
    g.setGradientFill(gradient);
    g.fillRect(getWidth() / 5.0, getHeight() * (1-lvlLeft), getWidth() / 5.0,  lvlLeft * getHeight());
}

void VUMeter::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
