/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================

MainComponent::MainComponent() : minOut(-1.0), maxOut(1.0)
{
    // Make sure you set the size of the component after
    // you add any child components

    //    addAndMakeVisible(conn1);
    setAudioChannels(0, 2);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.

    HighResolutionTimer::stopTimer();
    Timer::stopTimer();

    for (auto sensel : sensels)
    {
        if (sensel->senselDetected)
        {
            sensel->shutDown();
        }
    }
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    fs = sampleRate;
    bufferSize = samplesPerBlockExpected;

    for (int i = 0; i < amountOfSensels; i++)
    {
        sensels.add(new Sensel(i)); // chooses the device in the sensel device list
    }
    vector<ObjectType> objects{bowedString, bowedString, bowedString, bowedString, bowedString, plate};
    
    instruments.add(new Instrument(objects, fs));
    addAndMakeVisible(instruments[0]);
    
    numInstruments = instruments.size();
    // start the hi-res timer
    HighResolutionTimer::startTimer(1000.0 / 150.0);
    Timer::startTimerHz(15);
    setSize(1440, 900);
}

void MainComponent::hiResTimerCallback()
{
    double maxVb = 0.2;
    for (auto sensel : sensels)
    {
        if (sensel->senselDetected)
        {
            sensel->check();

            unsigned int fingerCount = sensel->contactAmount;
            int index = sensel->senselIndex;
            if (index == 0)
            {
                for (int f = 0; f < fingerCount; f++)
                {
                    if (sensel->fingers[f].x >= 0.7)
                    {
                        state[index] = sensel->fingers[f].state;
                        xpos[index] = sensel->fingers[f].x;

                        ypos[index] = sensel->fingers[f].y;
                        Vb[index] = sensel->fingers[f].delta_y * maxVb;
                        Fb[index] = sensel->fingers[f].force * 1000;
                    }
                    else
                    {
                        if (sensel->fingers[f].y < 0.1)
                            connectionPoint[index] = sensel->fingers[f].x;
                        else
                            fp[index] = sensel->fingers[f].x;
                    }
                }
                instruments[0]->getStrings()[index]->setBow(state[index]);
                instruments[0]->getStrings()[index]->setVb(Vb[index]);
                instruments[0]->getStrings()[index]->setFb(Fb[index]);
                instruments[0]->getStrings()[index]->setBowPos(xpos[index], ypos[index]);
                instruments[0]->getStrings()[index]->setFingerPoint(fp[index]);
                instruments[0]->getStrings()[index]->setConnection(0, connectionPoint[index]);
            } else if (index == 1)
            {
                instruments[0]->getPlates()[0]->setImpactPosition (sensel->fingers[0].x, sensel->fingers[0].y);
                instruments[0]->getPlates()[0]->setInput (sensel->fingers[0].force * 1000);
            }
//                instrument->getConnections()[0].setCP(index, connectionPoint[index]);
        }
    }
}

void MainComponent::getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill)
{

    float *const channelData1 = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
    float *const channelData2 = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);

    vector<double> output{0.0, 0.0};

    for (int i = 0; i < bufferToFill.buffer->getNumSamples(); i++)
    {
        for (int j = 0; j < numInstruments; ++j)
        {
            output = instruments[j]->calculateOutput();
        }
        channelData1[i] = clip(output[0]);
        channelData2[i] = clip(output[1]);
    }
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint(Graphics &g)
{
    g.setColour(Colours::lightgrey);
    g.drawLine(0, getHeight() / 2.0, getWidth(), getHeight() / 2.0);
}

void MainComponent::resized()
{
    instruments[0]->setBounds(getLocalBounds());
}

float MainComponent::clip(float output)
{
    if (output > maxOut)
    {
        return output = maxOut;
    }
    else if (output < minOut)
    {
        return output = minOut;
    }
    return output;
}

void MainComponent::timerCallback()
{
    repaint();
}
