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

    setSize(1440, 900);

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
    fs = sampleRate;
    bufferSize = samplesPerBlockExpected;

    double frequencyInHz[] = {110.0, 110.0 * pow(2, 7.0 / 12.0)};

    for (int i = 0; i < numStrings; ++i)
    {
        violinStrings.add(new ViolinString(frequencyInHz[i], fs));

        int lengthInPixels = (int)(getWidth() - 40); /// (frequencyInHz[i] / 110.0));
                                                     //        auto c = Colour::fromHSV(Random().nextFloat(), 0.6f, 0.9f, 1.0f);
        auto c = Colours::cyan;
        stringLines.add(new StringAnimation(lengthInPixels, c, getHeight() / 2.0));
        addAndMakeVisible(stringLines[i]);
    }
    
//    violinStrings[0]->setRaisedCosSinglePoint(0.5);
    for (int i = 0; i < amountOfSensels; i++)
        sensels.add(new Sensel(i)); // chooses the device in the sensel device list

    Connection conn(violinStrings[0], violinStrings[1],
                    0.5, 0.4,
                    1, 1,
                    1, 10, 1000000, fs);
    conn1 = conn;
    resized();
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
                        connectionPoint[index] = sensel->fingers[f].x;
                    else
                        fingerPoint[index] = sensel->fingers[f].x;
                
                }
            }

            violinStrings[index]->setBow(state[index]);
            violinStrings[index]->setVb(Vb[index]);
            violinStrings[index]->setFb(Fb[index]);
            violinStrings[index]->setBowPos(xpos[index]);
            violinStrings[index]->setFingerPoint(fingerPoint[index]);
           
            if (index == 0)
                conn1.setCPIdx1(violinStrings[index]->getNumPoints() * connectionPoint[index]);
            else
                conn1.setCPIdx2(violinStrings[index]->getNumPoints() * connectionPoint[index]);

            if (state[index])
            {
                auto position = xpos[index];

                stringLines[index]->stringPlucked(position);
            }
        }
    }

    if (stateUpdateCounter % 10 == 0)
    {
        for (int s = 0; s < numStrings; s++)
        {
            stringLines[s]->setFingerPoint(fingerPoint[s], violinStrings[s]->getNumPoints());
            stringLines[s]->updateStringStates(violinStrings[s]->getState(), xpos[s], ypos[s], Fb[s], conn1.getCPIdx()[s]);
        }
        //if (repaintFlag)
        //    repaint();
    }
    stateUpdateCounter++;
}

void MainComponent::getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill)
{

    float *const channelData1 = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
    float *const channelData2 = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);

    float output1 = 0.0;
    float output2 = 0.0;

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
        }

        output1 = violinStrings[0]->getOutput(0.25) * 600;
        output2 = violinStrings[1]->getOutput(0.25) * 600;
        
        channelData1[i] = clip (output1);
        channelData2[i] = clip (output2);
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
    auto xPos = 20;
    auto yPos = 0;
    auto yDistance = getHeight() / 2.0;

    for (auto stringLine : stringLines)
    {
        stringLine->setTopLeftPosition(xPos, yPos);
        yPos += yDistance;
    }
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

    double bp = e.x < getWidth() ? e.x / static_cast<double>(getWidth()) : 1;
    double fp = e.x < getWidth() ? e.x / static_cast<double>(getWidth()) : 1;
    if (bp <= 0)
    {
        bp = 0;
    }
    if (fp <= 0)
    {
        fp = 0;
    }

    if (e.y < getHeight() / 2.0)
    {
        if (ModifierKeys::getCurrentModifiers() == ModifierKeys::altModifier + ModifierKeys::leftButtonModifier)
        {
            double cp = e.x < getWidth() ? violinStrings[0]->getNumPoints() * e.x / static_cast<double>(getWidth()) : violinStrings[0]->getNumPoints();
            if (cp <= 0)
            {
                cp = 0;
            }
            conn1.setCPIdx1(cp);
        }
        else if (ModifierKeys::getCurrentModifiers() == ModifierKeys::ctrlModifier + ModifierKeys::leftButtonModifier)
        {
            violinStrings[0]->setFingerPoint(fp);
            stringLines[0]->setFingerPoint(fp, violinStrings[0]->getNumPoints());
        }
        else
        {
            double Vb = (e.y - getHeight() * 0.25) / (static_cast<double>(getHeight() * 0.25)) * maxVb;
            violinStrings[0]->setVb(Vb);
            violinStrings[0]->setBowPos(bp);
        }
    }
    else
    {
        if (ModifierKeys::getCurrentModifiers() == ModifierKeys::altModifier + ModifierKeys::leftButtonModifier)
        {
            double cp = e.x < getWidth() ? violinStrings[1]->getNumPoints() * e.x / static_cast<double>(getWidth()) : violinStrings[1]->getNumPoints();
            if (cp <= 0)
            {
                cp = 0;
            }
            conn1.setCPIdx2(cp);
            //            conn1.setCPIdx2(violinStrings[1]->getNumPoints() * e.x / static_cast<double>(getWidth()));
        }
        else if (ModifierKeys::getCurrentModifiers() == ModifierKeys::ctrlModifier + ModifierKeys::leftButtonModifier)
        {
            violinStrings[1]->setFingerPoint(fp);
            stringLines[1]->setFingerPoint(fp, violinStrings[1]->getNumPoints());
        }
        else
        {
            double Vb = (e.y - getHeight() * 0.75) / (static_cast<double>(getHeight() * 0.25)) * maxVb;
            violinStrings[1]->setVb(Vb);
            violinStrings[1]->setBowPos(bp);
        }
    }
}

void MainComponent::mouseUp(const MouseEvent &e)
{
    for (auto string : violinStrings)
    {
        string->setBow(false);
    }
}
