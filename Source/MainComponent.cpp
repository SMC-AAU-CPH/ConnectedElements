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
    // you add any child components.

    setSize(800, 600);

    // specify the number of input and output channels that we want to open
    setAudioChannels(0, 2);
    startTimer(1000.0 / 150.0);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()

    fs = sampleRate;
    bufferSize = samplesPerBlockExpected;

    violinStrings.add(new ViolinString(110.0, fs));
    violinStrings.add(new ViolinString(110.0 * pow(2, 7.0 / 12.0), fs));

    for (int i = 0; i < amountOfSensels; i++)
        sensels.add(new Sensel(i)); // chooses the device in the sensel device list
    
    Connection conn(violinStrings[0], violinStrings[1],
                    0.2, 0.4,
                    1, 1,
                    1, 100, 100, fs);
    conn1 = conn;
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

                state[index] = sensel->fingers[f].state;
                xpos[index] = sensel->fingers[f].x;
                ypos[index] = sensel->fingers[f].y;
                Vb[index] = sensel->fingers[f].delta_y * maxVb;
                Fb[index] = sensel->fingers[f].force * 1000;
            }

            violinStrings[index]->setBow(state[index]);
            violinStrings[index]->setVb(Vb[index]);
            violinStrings[index]->setFb(Fb[index]);
            violinStrings[index]->setBowPos(xpos[index]);
        }
    }

    //    for (int i = 0; i < numStrings; i++)
    //    {
    ////        violinStrings[i]->setBow(state[i]);
    ////        violinStrings[i]->setVb(Vb[i]);
    ////        violinStrings[i]->setFb(Fb[i]);
    ////        violinStrings[i]->setBowPos(xpos[i]);
    //    }
}

void MainComponent::getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill)
{
    for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
    {
        float *const channelData = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);

        if (channel == 0)
        {
            for (int i = 0; i < bufferToFill.buffer->getNumSamples(); i++)
            {

                float output = 0.0;
                conn1.calculateCoefs();
                for (int j = 0; j < numStrings; ++j)
                {
                    violinStrings[j]->bow();
                }
                
                for (int j = 0; j < numStrings; ++j)
                {
                    double JFc = conn1.calculateJFc()[j];
                    violinStrings[j]->addJFc(JFc, conn1.getCPIdx()[j]);
                    violinStrings[j]->updateUVectors();
                    output = output + violinStrings[j]->getOutput(0.25) * 600;
                }
                

                if (output > maxOut)
                {
                    output = maxOut;
                }
                else if (output < minOut)
                {
                    output = minOut;
                }
                channelData[i] = output;
            }
        }
        else
        {
            memcpy(channelData,
                   bufferToFill.buffer->getReadPointer(0),
                   bufferToFill.numSamples * sizeof(float));
        }
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
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

    for (int i = 0; i < numStrings; ++i)
    {
        g.setColour(Colour::fromRGB(50 + i * 200.0 / static_cast<double>(numStrings), 0, 0));
        g.fillRect(0, 0, getWidth(), getHeight()/2.0);
        g.setColour(Colours::grey);
        g.drawRect(0, getHeight() / 2.0, getWidth(), getHeight() / 2.0);
    }
    // You can add your drawing code here!
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}

void MainComponent::mouseDown(const MouseEvent &e)
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

void MainComponent::mouseDrag(const MouseEvent &e)
{
    double maxVb = 0.2;
    double Vb;
    double bp;
    if (e.y < getHeight() / 2.0)
    {
        Vb = (e.y - getHeight() * 0.25) / (static_cast<double>(getHeight() * 0.25)) * maxVb;
        bp = e.x / static_cast<double>(getWidth());
        violinStrings[0]->setVb(Vb);
        violinStrings[0]->setBowPos(bp);
    //    double Fb = e.x / (static_cast<double>(getWidth())) * 100;
    } else {
        Vb = (e.y - getHeight() * 0.75) / (static_cast<double>(getHeight() * 0.25)) * maxVb;
        bp = e.x / static_cast<double>(getWidth());
        violinStrings[1]->setVb(Vb);
        violinStrings[1]->setBowPos(bp);
    }
}

void MainComponent::mouseUp(const MouseEvent &e)
{
    for (int j = 0; j < numStrings; ++j)
    {
        violinStrings[j]->setBow(false);
    }
}
