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

    double frequencyInHz[] = {110.0, 110.0 * pow(2, 7.0 / 12.0)};
    
//    addAndMakeVisible(conn1);
    
    for (int i = 0; i < numStrings; ++i)
    {
        violinStrings.add (new ViolinString(frequencyInHz[i], 44100));
        addAndMakeVisible (violinStrings[i]);
    }

    
    setSize(1440, 900);
    setAudioChannels(0, 2);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    stopTimer();
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    fs = sampleRate;
    bufferSize = samplesPerBlockExpected;

    for (int i = 0; i < amountOfSensels; i++)
        sensels.add(new Sensel(i)); // chooses the device in the sensel device list

    cp = {0.95, 0.5};
    conn1.setCoeffs (violinStrings[0], violinStrings[1],
                     cp[0], cp[1],
                     1, 1,
                     1, 500, 10, fs);
    // start the hi-res timer
    startTimer(1000.0 / 150.0);
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
            for (int f = 0; f < fingerCount; f++)
            {
                if (f == 0)
                {
                    state[index] = sensel->fingers[f].state;
                    xpos[index] = sensel->fingers[f].x;
                    
                    ypos[index] = sensel->fingers[f].y;
                    Vb[index] = sensel->fingers[f].delta_y * maxVb;
                    Fb[index] = sensel->fingers[f].force * 1000;
                }

                if (f == 1)
                {
                    if (sensel->fingers[f].y < 0.1)
                        cp[index] = sensel->fingers[f].x;
                    else
                        fp[index] = sensel->fingers[f].x;
                
                }
            }

            violinStrings[index]->setBow(state[index]);
            violinStrings[index]->setVb(Vb[index]);
            violinStrings[index]->setFb(Fb[index]);
            violinStrings[index]->setBowPos(xpos[index], ypos[index]);
            violinStrings[index]->setFingerPoint(fp[index]);
            
            conn1.setCP(index, cp[index]);
        }
    }

    if (stateUpdateCounter % 10 == 0)
    {
        for (int s = 0; s < numStrings; s++)
        {
            violinStrings[s]->setConnection (cp[s]);
        }
    }
    stateUpdateCounter++;
    
}

void MainComponent::getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill)
{

    float *const channelData1 = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
    float *const channelData2 = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);

    vector<float> output = {0.0, 0.0};

    for (int i = 0; i < bufferToFill.buffer->getNumSamples(); i++)
    {

        conn1.calculateCoefs();
        for (int j = 0; j < numStrings; ++j)
        {
            violinStrings[j]->bow();
        }

        vector<double> JFc = conn1.calculateJFc();
        for (int j = 0; j < numStrings; ++j)
        {
            violinStrings[j]->addJFc(JFc[j], conn1.getCPIdx()[j]);
            violinStrings[j]->updateUVectors();
            output[j] = violinStrings[j]->getOutput(0.75) * 600;
        }
        channelData1[i] = clip (output[0]);
        channelData2[i] = clip (output[1]);
        
    }
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

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint(Graphics &g)
{
    for (int i = 0; i < numStrings; ++i)
    {
        g.setColour(Colour::fromRGBA(50 + i * 200.0 / static_cast<double>(numStrings), 0, 0, 0.2));
        g.fillRect(0, 0, getWidth(), getHeight() / 2.0);
        g.setColour(Colours::grey);
        g.drawRect(0, getHeight() / 2.0, getWidth(), getHeight() / 2.0);
    }
}

void MainComponent::resized()
{
    conn1.setBounds(getLocalBounds());
    violinStrings[0]->setBounds(0, 0, getWidth(), getHeight() / 2.0);
    violinStrings[1]->setBounds(0, getHeight() / 2.0, getWidth(), getHeight() / 2.0);
}

void MainComponent::mouseDown(const MouseEvent &e)
{
    if (ModifierKeys::getCurrentModifiers() == ModifierKeys::leftButtonModifier)
    {
        if (e.y < getHeight() / 2.0)
        {
            violinStrings[0]->setBow(true);
        }
        else
        {
            violinStrings[1]->setBow(true);
        }
    }
}

void MainComponent::mouseDrag(const MouseEvent &e)
{
    double maxVb = 0.2;
    int idx;
    if (e.y < getHeight() / 2.0)
    {
        idx = 0;
    } else {
        idx = 1;
    }
    if (ModifierKeys::getCurrentModifiers() == ModifierKeys::altModifier + ModifierKeys::leftButtonModifier)
    {
        cp[idx] = e.x <= 0 ? 0 : (e.x < getWidth() ? e.x / static_cast<double>(getWidth()) : 1);
        conn1.setCP(idx, cp[idx]);
        violinStrings[idx]->setConnection (cp[idx]);
    }
    else if (ModifierKeys::getCurrentModifiers() == ModifierKeys::ctrlModifier + ModifierKeys::leftButtonModifier)
    {
        fp[idx] = e.x <= 0 ? 0 : (e.x < getWidth() ? e.x / static_cast<double>(getWidth()) : 1);
        violinStrings[idx]->setFingerPoint(fp[idx]);
    }
    else
    {
        double Vb = (e.y - getHeight() * (idx == 0 ? 0.25 : 0.75)) / (static_cast<double>(getHeight() * 0.25)) * maxVb;
        violinStrings[idx]->setVb(Vb);
        bpX[idx] = e.x <= 0 ? 0 : (e.x < getWidth() ? e.x / static_cast<double>(getWidth()) : 1);
        if (idx == 0)
        {
            bpY[idx] = e.y <= 0 ? 0 : (e.y < getHeight() / 2.0 ? e.y / static_cast<double>(getHeight() / 2.0) : 1);
        } else {
            bpY[idx] = e.y <= getHeight() / 2.0 ? 0 : (e.y < getHeight() ? (e.y - (getHeight() / 2.0)) / static_cast<double>(getHeight() / 2.0) : 1);
        }
        violinStrings[idx]->setBowPos(bpX[idx], bpY[idx]);
    }
}

void MainComponent::mouseUp(const MouseEvent &e)
{
    for (auto string : violinStrings)
    {
        string->setBow(false);
    }
}
