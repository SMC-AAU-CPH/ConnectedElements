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

    dist.setSamplingRate(fs);

    for (int i = 0; i < amountOfSensels; i++)
    {
        sensels.add(new Sensel(i)); // chooses the device in the sensel device list
    }
    chooseInstrument = 1;
    if (chooseInstrument == 0)
    {
        vector<ObjectType> objects{bowedString, bowedString};
        int stringPlateDivision = 800;
        instruments.add(new Instrument(objects, fs, stringPlateDivision));
    }
    else if (chooseInstrument == 1)
    {
        vector<ObjectType> objects{bowedString, bowedString,
                                   sympString, sympString, sympString, sympString,
                                   plate};

        int stringPlateDivision = 3 * 800 / 4.0;
        instruments.add(new Instrument(objects, fs, stringPlateDivision));
    }

    addAndMakeVisible(instruments[0]);

    numInstruments = instruments.size();
    // start the hi-res timer
    HighResolutionTimer::startTimer(1000.0 / 150.0);
    Timer::startTimerHz(15);
    setSize(1440, 800);
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
            if (chooseInstrument == 0)
            {
                for (int f = 0; f < fingerCount; f++)
                {

                    bool state = sensel->fingers[f].state;
                    float x = sensel->fingers[f].x;
                    float y = sensel->fingers[f].y;
                    float Vb = sensel->fingers[f].delta_y * maxVb;
                    float Fb = sensel->fingers[f].force * 1000;
                    int fingerID = sensel->fingers[f].fingerID;

                    if (f == 0 && state) //fingerID == 0)
                    {

                        instruments[0]->getStrings()[index]->setBow(state);
                        instruments[0]->getStrings()[index]->setVb(Vb);
                        instruments[0]->getStrings()[index]->setFb(Fb);
                        instruments[0]->getStrings()[index]->setBowPos(x, y);
                    }
                    else if (fingerID > 0)
                    {
                        instruments[0]->getStrings()[index]->setFingerPosition(x);
                    }
                }

                if (fingerCount == 0)
                {
                    instruments[0]->getStrings()[index]->setBow(false);
                }
            }
            else if (chooseInstrument == 1)
            {
                if (index == 0)
                {
                    for (int f = 0; f < fingerCount; f++)
                    {

                        bool state = sensel->fingers[f].state;
                        float x = sensel->fingers[f].x;
                        float y = sensel->fingers[f].y;
                        float Vb = sensel->fingers[f].delta_y * maxVb;
                        float Fb = sensel->fingers[f].force * 1000;
                        int fingerID = sensel->fingers[f].fingerID;

                        float range = 1.0 / static_cast<float>(instruments[0]->getNumBowedStrings());
                        if (f == 0 && state) //fingerID == 0)
                        {
                            unsigned int pickAString = 0;
                            for (int j = 0; j < instruments[0]->getNumBowedStrings(); ++j)
                                if (y > (range * j) && y < range * (j + 1))
                                    pickAString = j;

                            for (int ps = 0; ps < instruments[0]->getNumBowedStrings(); ps++)
                            {
                                if (ps == pickAString)
                                {
                                    instruments[0]->getStrings()[ps]->setBow(state);
                                    instruments[0]->getStrings()[ps]->setVb(Vb);
                                    instruments[0]->getStrings()[ps]->setFb(Fb);
                                    instruments[0]->getStrings()[ps]->setBowPos(x, y);
                                }
                                else
                                    instruments[0]->getStrings()[ps]->setBow(false);
                            }
                            //instruments[0]->getStrings()[f]->setFingerPoint(fp[index]);
                        }
                        else if (fingerID > 0)
                        {
                            unsigned int pickAString = 0;
                            for (int j = 0; j < instruments[0]->getNumBowedStrings(); ++j)
                                if (y > (range * j) && y < range * (j + 1))
                                    pickAString = j;

                            for (int ps = 0; ps < instruments[0]->getNumBowedStrings(); ps++)
                            {
                                if (ps == pickAString)
                                {
                                    instruments[0]->getStrings()[ps]->setFingerPosition(x);
                                }
                                //else
                                //instruments[0]->getStrings()[ps]->setFingerPosition(0);;
                            }
                        }
                    }

                    if (fingerCount == 0)
                    {
                        for (auto violinString : instruments[0]->getStrings())
                            violinString->setBow(false);
                    }
                }

                if (index == 1)
                {
                    if (fingerCount == 0)
                    {
                        for (int i = instruments[0]->getNumBowedStrings(); i < instruments[0]->getNumBowedStrings() + instruments[0]->getNumSympStrings(); ++i)
                            instruments[0]->getStrings()[i]->pick(false);
                    }
                    vector<bool> pickAString (4, false);
                    vector<double> forces (4, 0);
                    vector<double> xPositions (4, 0);
                    
                    int bowedStringsAmount = instruments[0]->getNumBowedStrings();
                    int sympStringsAmount = instruments[0]->getNumSympStrings();
                    int totalStringsAmount = instruments[0]->getNumSympStrings() + instruments[0]->getNumBowedStrings();
                    
                    float range = 1.0 / static_cast<float>(sympStringsAmount);
                    
                    for (int f = 0; f < fingerCount; f++)
                    {
                        bool state = sensel->fingers[f].state;
                        float x = sensel->fingers[f].x;
                        float y = sensel->fingers[f].y;
                        float Vb = sensel->fingers[f].delta_y * maxVb;
                        float Fb = sensel->fingers[f].force * 1000;
                        int fingerID = sensel->fingers[f].fingerID;
        
                        
                        for (int j = 0; j < sympStringsAmount; ++j)
                            if (x > (range * j) && x < range * (j + 1))
                            {
                                pickAString[j] = true;
                                forces[j] = Fb;
                                xPositions[j] = y;
                            }
                        
                        
                        
                    }
                    for (int i = 0; i < sympStringsAmount; ++i)
                    {
                        if (pickAString[i])
                        {
                            if (!instruments[0]->getStrings()[i+bowedStringsAmount]->isPicking())
                            {
                                instruments[0]->getStrings()[i+bowedStringsAmount]->setRaisedCos(xPositions[i], 5, forces[i] / 500.0);
                                instruments[0]->getStrings()[i+bowedStringsAmount]->pick(true);
                            }
                        } else {
                            instruments[0]->getStrings()[i+bowedStringsAmount]->pick(false);
                        }
                    }
                }
            }
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
        output[0] = dist.getOutput(output[0]);
        
        channelData1[i] = clip(output[0]);
        channelData2[i] = clip(output[0]);
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
