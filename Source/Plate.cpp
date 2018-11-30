/*
  ==============================================================================

    Plate.cpp
    Created: 29 Nov 2018 9:21:27am
    Author:  Silvin Willemsen

  ==============================================================================
*/

#include "Plate.h"
Plate::Plate (double fs)
{
    sigma0 = 0.1;
    sigma1 = 0.005;
    setSampleRate(fs);
}

void Plate::setSampleRate (double sampleRate)
{
    fs = sampleRate;
    k = 1.0f / sampleRate;
    
    for (int x = 0; x < Nx; x++)
    {
        for (int y = 0; y < Ny; y++)
        {
            u[x][y] = 0.0f;
            un[x][y] = 0.0f;
            un1[x][y] = 0.0f;
            if (x == strikePositionX && y == strikePositionY)
                excitationArea[x][y] = 1.0f;
            else
                excitationArea[x][y] = 0.0f;
        }
    }
    
    setFrequency(220);
}

void Plate::excite()
{
    double excitation = input * 10;
    
    for (int l = 2; l < Nx - 2; l++)
    {
        for (int m = 2; m < Ny - 2; m++)
        {
            u[l][m] =
            B1 * (un[l][m + 2] + un[l][m - 2] + un[l + 2][m] + un[l - 2][m])
            + B2 * (un[l + 1][m + 1] + un[l - 1][m + 1] + un[l + 1][m - 1] + un[l - 1][m - 1])
            + B3 * (un[l][m + 1] + un[l][m - 1] + un[l + 1][m] + un[l - 1][m]) + C1 * un[l][m]
            + C2 * un1[l][m]
            + C3 * (un[l][m + 1] + un[l][m - 1] + un[l + 1][m] + un[l - 1][m] - un1[l][m + 1] - un1[l][m - 1] - un1[l + 1][m] - un1[l - 1][m])
            + C4 * excitationArea[l][m] * excitation;
        }
    }
    
    input = 0.0;
}

float Plate::getOutput (float ratioX, float ratioY)
{
    int x = clamp(floor(ratioX * Nx), 2, Nx - 2);
    int y = clamp(floor(ratioY * Ny), 2, Ny - 2);
    return clamp(u[x][y] * 1000, -1.0, 1.0);
}

void Plate::setImpactPosition (float xPos, float yPos)
{
    auto pointX = xPos * Nx;
    auto pointY = yPos * Ny;
    strikePositionX = clamp(pointX, 2, Nx - 2); // clamp position between 2 -> 7
    strikePositionY = clamp(pointY, 2, Ny - 2); // clamp position between 2 -> 7
    
    for (int x = 0; x < Nx; x++)
    {
        for (int y = 0; y < Ny; y++)
        {
            
            //if ((x == strikePositionX || x == strikePositionX + 1) && (y == strikePositionY || y == strikePositionY + 1))
            if (x == strikePositionX && y == strikePositionY)
                excitationArea[x][y] = 1.0f;
            else
                excitationArea[x][y] = 0.0f;
        }
    }
}

void Plate::setFrequency (float f)
{
    frequency = clamp(f, 30, 1300); // not really precise frequency here yet
    
    kappa = double(frequency * (27.15 / 110) * 4); // Stiffness of plate
    
    d = 1.0f / (1.0f + sigma0 * k);
    B1 = -(kappa * (k * k)) / (h * h * h * h) * d;
    B2 = B1 * 2.0f;
    B3 = B1 * -8.0f;
    C = (2.0f * sigma1 * k) / (h * h);
    C1 = (2.0f - 4.0f * C + 20.0f * B1) * d;
    C2 = (sigma0 * k - 1.0f + 4.0f * C) * d;
    C3 = C * d;
    C4 = (k * k) * d;
}

int Plate::addConnection (tuple<double, double> cp)
{
    auto [x, y] = cp;
    cpIdx.push_back(make_tuple(floor(x * Nx), floor(y * Ny)));
    return static_cast<int> (cpIdx.size() - 1);
}

void Plate::setDamping (float frequencyDependent, float frequencyIndependent)
{
    sigma1 = clamp(frequencyDependent, 0.0001, 0.5);    // Frequency dependent damping
    sigma0 = clamp(frequencyIndependent, 0.0001, 10.0); // Frequency independent damping
}

void Plate::updateUVectors()
{
    for (int l = 0; l < Nx; l++)
    {
        for (int m = 0; m < Ny; m++)
        {
            un1[l][m] = un[l][m];
            un[l][m] = u[l][m];
        }
    }
}

void Plate::addJFc(double JFc, tuple<int, int> cpIdx)
{
    auto [x, y] = cpIdx;
    u[x][y] = u[x][y] + JFc;
}

double Plate::clamp (double in, double min, double max)
{
    if (in > max)
        return max;
    else if (in < min)
        return min;
    else
        return in;
}

void Plate::paint (Graphics& g)
{

    int stateWidth = getWidth() / static_cast<double> (Nx - 4);
    int stateHeight = getHeight() / static_cast<double> (Ny - 4);
    int scaling = 10000;
    
    for (int x = 2; x < Nx - 2; ++x)
    {
        for (int y = 2; y < Ny - 2; ++y)
        {
            int cVal = clamp (255 * 0.5 * (u[x][y] * scaling + 1), 0, 255);
            g.setColour(Colour::fromRGBA (cVal, cVal, cVal, 127));
            g.fillRect((x - 2) * stateWidth, (y - 2) * stateHeight, stateWidth, stateHeight);
            for (int c = 0; c < cpIdx.size(); ++c)
            {
                auto [cpX, cpY] = cpIdx[c];
                if (x == cpX && y == cpY)
                {
                    g.setColour(Colours::orange);
                    g.drawRect((x - 2) * stateWidth, (y - 2) * stateHeight, stateWidth, stateHeight);
                }
            }
                
        }
    }
}

void Plate::resized()
{
}

void Plate::mouseDrag (const MouseEvent& e)
{
    int stateWidth = getWidth() / static_cast<double> (Nx - 4);
    int stateHeight = getHeight() / static_cast<double> (Ny - 4);
    int idX = e.x / stateWidth + 2;
    int idY = e.y / stateHeight + 2;
    
    if (ModifierKeys::getCurrentModifiers() == ModifierKeys::altModifier + ModifierKeys::leftButtonModifier)
    {
        cpIdx[0] = make_tuple(idX, idY);
    }
    else if (ModifierKeys::getCurrentModifiers() == ModifierKeys::altModifier + ModifierKeys::rightButtonModifier)
    {
        cpIdx[1] = make_tuple(idX, idY);
    }
    else
    {
        for (int x = 2; x < Nx - 2; x++)
        {
            for (int y = 2; y < Ny - 2; y++)
            {
                if (x == idX && y == idY)
                    excitationArea[x][y] = 1.0f;
                else
                    excitationArea[x][y] = 0.0f;
            }
        }
        input = 5000.0;
    }
}
