/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================

MainComponent::MainComponent() : minOut(-1.0), maxOut(1.0), keyboardComponent (keyboardState, MidiKeyboardComponent::horizontalKeyboard)
{
    // Make sure you set the size of the component after
    // you add any child components

    //    addAndMakeVisible(conn1);
    setAudioChannels(0, 2);
    
    addAndMakeVisible(keyboardComponent);
    keyboardState.addListener (this);
    
    addAndMakeVisible (midiInputList);
    midiInputList.setTextWhenNoChoicesAvailable ("No MIDI Inputs Enabled");
    auto midiInputs = MidiInput::getDevices();
    midiInputList.addItemList (midiInputs, 1);
    midiInputList.onChange = [this] { setMidiInput (midiInputList.getSelectedItemIndex()); };
    // find the first enabled device and use that by default
    for (auto midiInput : midiInputs)
    {
        if (deviceManager.isMidiInputEnabled (midiInput))
        {
            setMidiInput (midiInputs.indexOf (midiInput));
            break;
        }
    }
    // if no enabled devices were found just use the first one in the list
//    if (midiInputList.getSelectedId() == 0)
    setMidiInput (MidiInput::getDevices().size() - 1);
    
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
    delete plateStiffness;
    delete plateLabel;
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

    chooseInstrument = hurdyGurdy;

    int stringPlateDivision;
    int bowedSympDivision;
    vector<ObjectType> objects;
    vector<String> names;
    switch (chooseInstrument)
    {
    case twoStringViolin:
    {
        vector<ObjectType> preObjects = {bowedString, bowedString};
        objects = preObjects;
        int stringPlateDivision = appHeight;
        int bowedSympDivision = appWidth - controlsWidth;
        instruments.add(new Instrument(chooseInstrument, objects, fs, stringPlateDivision, bowedSympDivision));
        for (int i = 0; i < 2; ++i)
        {
            Slider *bowedStringSlider = new Slider();
            bowedStringSlider->setSliderStyle(Slider::LinearVertical);
            bowedStringSlider->setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
            bowedStringSlider->setPopupDisplayEnabled(true, false, this);
            Label *label = new Label("Str " + String(i), "Str " + String(i));
            label->setJustificationType(Justification::centred);
            addAndMakeVisible(label);
            labels.add(label);
            bowedStringSlider->setRange(0.0, 1.0, 0.1);
            bowedStringSlider->setValue(0.5);
            instruments[0]->setMix(i, 0.5);
            bowedStringSlider->addListener(this);
            addAndMakeVisible(bowedStringSlider);
            mixSliders.add(bowedStringSlider);
        }
        break;
    }
    case bowedSitar:
    {
        vector<ObjectType> preObjects{bowedString, bowedString,
                                      pluckedString, pluckedString, pluckedString, pluckedString, pluckedString,
                                      sympString, sympString, sympString, sympString,
                                      sympString, sympString, sympString, sympString,
                                      sympString, sympString, sympString, sympString,
                                      sympString,
                                      plate};
        objects = preObjects;
        stringPlateDivision = 0.75 * appHeight;
        bowedSympDivision = appHeight;
        names = {"Bow", "Pluck", "Symp", "Plate"};
        break;
    }

    case sitar:
    {
        vector<ObjectType> preObjects{pluckedString, pluckedString, pluckedString, pluckedString, pluckedString,
                                      sympString, sympString, sympString, sympString,
                                      sympString, sympString, sympString, sympString,
                                      sympString, sympString, sympString, sympString,
                                      sympString,
                                      plate};
        objects = preObjects;
        stringPlateDivision = 0.75 * appHeight;
        bowedSympDivision = 0;
        names = {"Pluck", "Symp", "Plate"};
        break;
    }

    case hurdyGurdy:
    {
        vector<ObjectType> preObjects{bowedString, bowedString, bowedString,
                                      sympString, sympString, sympString, sympString,
                                      sympString, sympString, sympString, sympString,
                                      sympString, sympString, sympString, sympString,
                                      sympString,
                                      plate};
        objects = preObjects;
        stringPlateDivision = 0.75 * appHeight;
        bowedSympDivision = appHeight;
        names = {"Bow", "Pluck", "Symp", "Plate"};
        break;
    }
    case dulcimer:
    {
        vector<ObjectType> preObjects{
            pluckedString, pluckedString,
            pluckedString, pluckedString,
            pluckedString, pluckedString,
            pluckedString, pluckedString,
            pluckedString, pluckedString,
            pluckedString, pluckedString,
            pluckedString, pluckedString,
            pluckedString, pluckedString,
            pluckedString, pluckedString,
            pluckedString, pluckedString,
            pluckedString, pluckedString,
            pluckedString, pluckedString,
            pluckedString, pluckedString,
            pluckedString, pluckedString,
            pluckedString, pluckedString,
            pluckedString, pluckedString,
            pluckedString, pluckedString,
            pluckedString, pluckedString,
            pluckedString, pluckedString,
            pluckedString, pluckedString,
            plate};
        objects = preObjects;
        stringPlateDivision = 0.75 * appHeight;
        bowedSympDivision = 0;
        names = {"Pluck", "Plate"};
        break;
    }
    }

    if (chooseInstrument != twoStringViolin)

    {
        // Everything except for the twoStringViolin will execute this code
        instruments.add(new Instrument(chooseInstrument, objects, fs, stringPlateDivision, bowedSympDivision));
        for (int i = 0; i < names.size(); ++i)
        {
            Slider *bowedStringSlider = new Slider();
            bowedStringSlider->setSliderStyle(Slider::LinearVertical);
            bowedStringSlider->setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
            bowedStringSlider->setPopupDisplayEnabled(true, false, this);
            Label *label = new Label(names[i], names[i]);
            label->setJustificationType(Justification::centred);
            addAndMakeVisible(label);
            labels.add(label);
            bowedStringSlider->setRange(0.0, 1.0, 0.1);
            bowedStringSlider->setValue(0.5);
            instruments[0]->setMix(i, 0.5);
            bowedStringSlider->addListener(this);
            addAndMakeVisible(bowedStringSlider);
            mixSliders.add(bowedStringSlider);
        }

        plateStiffness = new Slider(Slider::RotaryVerticalDrag, Slider::NoTextBox);
        plateStiffness->setPopupDisplayEnabled(true, false, this);
        plateStiffness->setRange(30, 1300, 1);
        plateStiffness->setValue(100);
        plateStiffness->addListener(this);
        addAndMakeVisible(plateStiffness);

        plateLabel = new Label("Plate Stiffness", "Plate Stiffness");
        plateLabel->setJustificationType(Justification::centred);
        addAndMakeVisible(plateLabel);
    }

    addAndMakeVisible(instruments[0]);

    // sensel stuff
    switch (chooseInstrument)
    {
    case twoStringViolin:
    {
        break;
    }
    case bowedSitar:
    {
        if (amountOfSensels == 2)
        {

            int pluckedStringsAmount = instruments[0]->getNumPluckedStrings();

            float range = 1.0 / static_cast<float>(pluckedStringsAmount);

            for (int i = 0; i < pluckedStringsAmount; i++)
            {
                sensels[1]->addLEDBrightness(range * i, 1);
            }
        }
        break;
    }

    case sitar:
    {
        if (amountOfSensels == 2)
        {

            int pluckedStringsAmount = instruments[0]->getNumPluckedStrings();

            float range = 1.0 / static_cast<float>(pluckedStringsAmount);

            for (int i = 0; i < pluckedStringsAmount; i++)
            {
                sensels[1]->addLEDBrightness(range * i, 1);
            }
        }
        break;
    }

    case hurdyGurdy:
    {
    }
    case dulcimer:
    {
        if (amountOfSensels == 2)
        {

            int pluckedStringsAmount = instruments[0]->getNumPluckedStrings() / 2;

            float range = 1.0 / static_cast<float>(pluckedStringsAmount);
            for (auto sensel : sensels)
            {
                for (int i = 0; i < pluckedStringsAmount; i += 2)
                {
                    sensel->addLEDBrightness(range * i, 1);
                }
            }
        }
        break;
    }
    }

    numInstruments = instruments.size();

    // start the hi-res timer
    if (sensels.size() != 0)
        if (sensels[0]->senselDetected)
            HighResolutionTimer::startTimer(1000.0 / 150.0);
    
    Timer::startTimerHz(15);
    setSize(appWidth, appHeight);
}
void MainComponent::hiResTimerCallback()
{
    switch (chooseInstrument)
    {
    case twoStringViolin:
        senselMappingTwoStrings();
        break;
    case bowedSitar:
        senselMappingSitarBowed();
        break;
    case sitar:
        senselMappingSitarPlucked();
        break;
    case hurdyGurdy:
        senselMappingHurdyGurdy();
        break;
    case dulcimer:
        senselMappingDulcimer();
        break;
    }
}

void MainComponent::senselMappingTwoStrings()
{
    double maxVb = 0.5;
    for (auto sensel : sensels)
    {
        if (sensel->senselDetected)
        {
            sensel->check();
            unsigned int fingerCount = sensel->contactAmount;
            int index = sensel->senselIndex;
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
    }
}
void MainComponent::senselMappingSitarBowed()
{
    double maxVb = 0.5;
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
                int bowedStringsAmount = instruments[0]->getNumBowedStrings();
                int pluckedStringsAmount = instruments[0]->getNumPluckedStrings();
                int sympStringsAmount = instruments[0]->getNumSympStrings();

                vector<bool> pickAString(sympStringsAmount, false);
                vector<double> forces(sympStringsAmount, 0);
                vector<double> xPositions(sympStringsAmount, 0);

                float range = 1.0 / static_cast<float>(pluckedStringsAmount);

                for (int f = 0; f < fingerCount; f++)
                {
                    float x = sensel->fingers[f].x;
                    float y = sensel->fingers[f].y;
                    float Fb = sensel->fingers[f].force;

                    for (int j = 0; j < pluckedStringsAmount; ++j)
                        if (x > (range * j) && x < range * (j + 1))
                        {
                            pickAString[j] = true;
                            forces[j] = Fb;
                            xPositions[j] = y;
                        }
                }
                for (int i = 0; i < pluckedStringsAmount; ++i)
                {
                    if (pickAString[i])
                    {
                        if (!instruments[0]->getStrings()[i + bowedStringsAmount]->isPicking())
                        {
                            instruments[0]->getStrings()[i + bowedStringsAmount]->setRaisedCos(xPositions[i], 5, forces[i] / 10.0);
                            instruments[0]->getStrings()[i + bowedStringsAmount]->pick(true);
                        }
                    }
                    else
                    {
                        instruments[0]->getStrings()[i + bowedStringsAmount]->pick(false);
                    }
                }
            }
        }
    }
}

void MainComponent::senselMappingSitarPlucked()
{
    double maxVb = 0.5;
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
                int bowedStringsAmount = instruments[0]->getNumBowedStrings();
                int pluckedStringsAmount = instruments[0]->getNumPluckedStrings();
                int sympStringsAmount = instruments[0]->getNumSympStrings();

                int totalStringsAmount = bowedStringsAmount + pluckedStringsAmount + sympStringsAmount;

                vector<bool> pickAString(sympStringsAmount, false);
                vector<double> forces(sympStringsAmount, 0);
                vector<double> xPositions(sympStringsAmount, 0);

                float range = 1.0 / static_cast<float>(pluckedStringsAmount);

                for (int f = 0; f < fingerCount; f++)
                {
                    bool state = sensel->fingers[f].state;
                    float x = sensel->fingers[f].x;
                    float y = sensel->fingers[f].y;
                    float Vb = sensel->fingers[f].delta_y * maxVb;
                    float Fb = sensel->fingers[f].force;
                    int fingerID = sensel->fingers[f].fingerID;

                    for (int j = 0; j < pluckedStringsAmount; ++j)
                        if (x > (range * j) && x < range * (j + 1))
                        {
                            pickAString[j] = true;
                            forces[j] = Fb;
                            xPositions[j] = y;
                        }
                }
                for (int i = 0; i < pluckedStringsAmount; ++i)
                {
                    if (pickAString[i])
                    {
                        if (!instruments[0]->getStrings()[i + bowedStringsAmount]->isPicking())
                        {
                            instruments[0]->getStrings()[i + bowedStringsAmount]->setRaisedCos(xPositions[i], 5, forces[i] / 10.0);
                            instruments[0]->getStrings()[i + bowedStringsAmount]->pick(true);
                        }
                    }
                    else
                    {
                        instruments[0]->getStrings()[i + bowedStringsAmount]->pick(false);
                    }
                }
            }
        }
    }
}
void MainComponent::senselMappingHurdyGurdy()
{
    const double maxVb = 0.2;
    for (auto sensel : sensels)
    {
        if (sensel->senselDetected)
        {
            sensel->check();

            const unsigned int fingerCount = sensel->contactAmount;
            const int index = sensel->senselIndex;

            if (index == 0)
            {
                for (int f = 0; f < fingerCount; f++)
                {

                    const bool state = sensel->fingers[f].state;
                    const float x = sensel->fingers[f].x;
                    const float y = sensel->fingers[f].y;
                    const float Vb = sensel->fingers[f].force * maxVb;
                    const float Fb = sensel->fingers[f].force * 1000;
                    const int fingerID = sensel->fingers[f].fingerID;

                    const float range = 1.0 / static_cast<float>(instruments[0]->getNumBowedStrings());

                    unsigned int pickAString = 0;
                    for (int j = 0; j < instruments[0]->getNumBowedStrings(); ++j)
                        if (y > (range * j) && y < range * (j + 1))
                            pickAString = j;

                    if (state) //fingerID == 0)
                    {

                        instruments[0]->getStrings()[pickAString]->setBow(state);
                        instruments[0]->getStrings()[pickAString]->setVb(Vb);
                        instruments[0]->getStrings()[pickAString]->setFb(Fb);
                        instruments[0]->getStrings()[pickAString]->setBowPos(x, y);
                    }
                    else
                        instruments[0]->getStrings()[pickAString]->setBow(false);
                }
            }

            if (fingerCount == 0)
            {
                for (auto violinString : instruments[0]->getStrings())
                    violinString->setBow(false);
            }
        }
    }
}

void MainComponent::senselMappingDulcimer()
{

    for (auto sensel : sensels)
    {
        if (sensel->senselDetected)
        {
            sensel->check();

            unsigned int fingerCount = sensel->contactAmount;
            int index = sensel->senselIndex;

            if (index == 0)
            {
                int pluckedStringsAmount = instruments[0]->getNumPluckedStrings() / 2;

                vector<bool> pickAString(pluckedStringsAmount, false);
                vector<double> forces(pluckedStringsAmount, 0);
                vector<double> xPositions(pluckedStringsAmount, 0);

                float range = 1.0 / (static_cast<float>(pluckedStringsAmount));

                for (int f = 0; f < fingerCount; f++)
                {
                    bool state = sensel->fingers[f].state;
                    float x = 1.0 - sensel->fingers[f].x;
                    float y = 1.0 - sensel->fingers[f].y;
                    //float Vb = sensel->fingers[f].delta_y * maxVb;
                    float Fb = sensel->fingers[f].force;
                    int fingerID = sensel->fingers[f].fingerID;

                    for (int j = 0; j < pluckedStringsAmount; j += 2)
                    {
                        if (x > (range * j) && x < range * (j + 2))
                        {
                            pickAString[j] = true;
                            forces[j] = Fb;
                            xPositions[j] = y;
                            pickAString[j + 1] = true;
                            forces[j + 1] = Fb;
                            xPositions[j + 1] = y;
                        }
                    }
                }
                for (int i = 0; i < pluckedStringsAmount; i += 2)
                {
                    int stringIndex = i;
                    if (pickAString[i] || pickAString[i + 1])
                    {

                        if (!instruments[0]->getStrings()[stringIndex]->isPicking() && !instruments[0]->getStrings()[stringIndex + 1]->isPicking())
                        {
                            instruments[0]->getStrings()[stringIndex]->setRaisedCos(xPositions[i], 5, forces[i] / 5.0);
                            instruments[0]->getStrings()[stringIndex]->pick(true);
                            instruments[0]->getStrings()[stringIndex + 1]->setRaisedCos(xPositions[i], 5, forces[i] / 5.0);
                            instruments[0]->getStrings()[stringIndex + 1]->pick(true);
                        }
                    }
                    else
                    {
                        instruments[0]->getStrings()[stringIndex]->pick(false);
                        instruments[0]->getStrings()[stringIndex + 1]->pick(false);
                    }
                }
            }

            if (index == 1)
            {
                int pluckedStringsAmount = instruments[0]->getNumPluckedStrings() / 2;

                vector<bool> pickAString(pluckedStringsAmount, false);
                vector<double> forces(pluckedStringsAmount, 0);
                vector<double> xPositions(pluckedStringsAmount, 0);

                float range = 1.0 / (static_cast<float>(pluckedStringsAmount));

                for (int f = 0; f < fingerCount; f++)
                {
                    bool state = sensel->fingers[f].state;
                    float x = sensel->fingers[f].x;
                    float y = sensel->fingers[f].y;
                    //float Vb = sensel->fingers[f].delta_y * maxVb;
                    float Fb = sensel->fingers[f].force;
                    int fingerID = sensel->fingers[f].fingerID;

                    for (int j = 0; j < pluckedStringsAmount; j += 2)
                    {
                        if (x > (range * j) && x < range * (j + 2))
                        {
                            pickAString[j] = true;
                            forces[j] = Fb;
                            xPositions[j] = y;
                            pickAString[j + 1] = true;
                            forces[j + 1] = Fb;
                            xPositions[j + 1] = y;
                        }
                    }
                }
                for (int i = 0; i < pluckedStringsAmount; i += 2)
                {
                    int stringIndex = pluckedStringsAmount + i;
                    if (pickAString[i] || pickAString[i + 1])
                    {

                        if (!instruments[0]->getStrings()[stringIndex]->isPicking() && !instruments[0]->getStrings()[stringIndex + 1]->isPicking())
                        {
                            instruments[0]->getStrings()[stringIndex]->setRaisedCos(xPositions[i], 5, forces[i] / 5.0);
                            instruments[0]->getStrings()[stringIndex]->pick(true);
                            instruments[0]->getStrings()[stringIndex + 1]->setRaisedCos(xPositions[i], 5, forces[i] / 5.0);
                            instruments[0]->getStrings()[stringIndex + 1]->pick(true);
                        }
                    }
                    else
                    {
                        instruments[0]->getStrings()[stringIndex]->pick(false);
                        instruments[0]->getStrings()[stringIndex + 1]->pick(false);
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
        //        std::cout << output[0] << std::endl;
        channelData1[i] = output[0];
        channelData2[i] = output[0];
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
    Rectangle<int> rect = getLocalBounds();
    Rectangle<int> controlsRect = rect.removeFromRight(controlsWidth);
    instruments[0]->setBounds(rect);

    int sliderWidth = controlsRect.getWidth() / 2;

    for (int row = 0; row < (mixSliders.size() + 1) / 2; ++row)
    {
        Rectangle<int> slidersArea = controlsRect.removeFromTop(150);
        Rectangle<int> labelsArea = slidersArea.removeFromTop(20);

        for (int i = row * 2; i < row * 2 + 2; ++i)
        {
            mixSliders[i]->setBounds(slidersArea.removeFromLeft(sliderWidth));
            labels[i]->setBounds(labelsArea.removeFromLeft(sliderWidth));
        }
    }

    if (instruments[0]->getNumPlates() != 0)
    {
        plateLabel->setBounds(controlsRect.removeFromTop(20));
        plateStiffness->setBounds(controlsRect.removeFromTop(controlsRect.getWidth()));
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

void MainComponent::timerCallback()
{
    repaint();
}

void MainComponent::sliderValueChanged(Slider *slider)
{
    for (int i = 0; i < mixSliders.size(); ++i)
    {
        if (mixSliders[i] == slider)
        {
            instruments[0]->setMix(i, slider->getValue());
        }
    }
    if (slider == plateStiffness)
    {
        instruments[0]->getPlates()[0]->setFrequency(slider->getValue());
    }
}

// MIDI
void MainComponent::setMidiInput(int index)
{
    auto list = MidiInput::getDevices();
    deviceManager.removeMidiInputCallback (list[lastInputIndex], this);
    auto newInput = list[index];
    if (! deviceManager.isMidiInputEnabled (newInput))
        deviceManager.setMidiInputEnabled (newInput, true);
    deviceManager.addMidiInputCallback (newInput, this);
    midiInputList.setSelectedId (index + 1, dontSendNotification);
    lastInputIndex = index;
}

void MainComponent::handleIncomingMidiMessage(MidiInput *source, const MidiMessage &message)
{
    const ScopedValueSetter<bool> scopedInputFlag (isAddingFromMidiInput, true);
    keyboardState.processNextMidiEvent (message);
    //    postMessageToList (message, source->getName());
    
}

void MainComponent::handleNoteOn(MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity)
{
    double i = midiNoteNumber - 60;
    
    double ratio = (1 - ((pow(2.0, ((12 - i) / 12.0))) - 1)) / 2.0;
    std::cout << ratio << std::endl;
    for (int i = 0; i < instruments[0]->getNumBowedStrings(); ++i)
    {
        instruments[0]->getStrings()[i]->setFingerPosition (ratio);
    }
}

void MainComponent::handleNoteOff(MidiKeyboardState* state, int midiChannel, int midiNoteNumber, float velocity)
{
    for (int i = 0; i < instruments[0]->getNumBowedStrings(); ++i)
    {
        instruments[0]->getStrings()[i]->setFingerPosition (0);
    }
}
