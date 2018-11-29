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
    _cpIdx.resize(2);
    fill(_cpIdx.begin(), _cpIdx.end(), 5);
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

float Plate::getOutput (double input)
{
    int l, m;
    
    double excitation = input * 1e7;
    
    for (l = 2; l < Nx - 2; l++)
    {
        for (m = 2; m < Ny - 2; m++)
        {
            //cout << "l: " << l << " m: " << m << "\n";
            u[l][m] =
            B1 * (un[l][m + 2] + un[l][m - 2] + un[l + 2][m] + un[l - 2][m]) + B2 * (un[l + 1][m + 1] + un[l - 1][m + 1] + un[l + 1][m - 1] + un[l - 1][m - 1]) + B3 * (un[l][m + 1] + un[l][m - 1] + un[l + 1][m] + un[l - 1][m]) + C1 * un[l][m] + C2 * un1[l][m] + C3 * (un[l][m + 1] + un[l][m - 1] + un[l + 1][m] + un[l - 1][m] - un1[l][m + 1] - un1[l][m - 1] - un1[l + 1][m] - un1[l - 1][m]) + C4 * excitationArea[l][m] * excitation;
        }
    }
    
    for (l = 0; l < Nx; l++)
    {
        for (m = 0; m < Ny; m++)
        {
            un1[l][m] = un[l][m];
            un[l][m] = u[l][m];
            //cout << "l: " << x << " m: " << y << "\n";
            //cout << "u[" << x << "]" << "[" << y << "]: " << u[x][y] << "\n";
        }
    }
    
    return clamp(u[outputPositionX][outputPositionY], -1.0, 1.0);
}

void Plate::setImpactPosition (float xPos, float yPos)
{
    auto pointX = xPos * Nx;
    auto pointY = yPos * Ny;
    strikePositionX = clamp(pointX, 2, 7); // clamp position between 2 -> 7
    strikePositionY = clamp(pointY, 2, 7); // clamp position between 2 -> 7
    
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
    
    // h = double(1.0f / Nx);
    
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

void Plate::setDamping (float frequencyDependent, float frequencyIndependent)
{
    sigma1 = clamp(frequencyDependent, 0.0001, 0.5);    // Frequency dependent damping
    sigma0 = clamp(frequencyIndependent, 0.0001, 10.0); // Frequency independent damping
}

double Plate::clamp (double input, double min, double max)
{
    if (input > max)
        return max;
    else if (input < min)
        return min;
    else
        return input;
}

void Plate::paint (Graphics& g)
{
//    g.fillAll(Colours::grey);
    int stateWidth = getWidth() / static_cast<double> (Nx);
    int stateHeight = getHeight() / static_cast<double> (Ny);

    for (int x = 0; x < Nx; ++x)
    {
        for (int y = 0; y < Ny; ++y)
        {
            g.setColour(Colour::fromRGB(255 * 0.5 * (u[x][y] + 1), 255 * 0.5 * (u[x][y] + 1), 255 * 0.5 * (u[x][y] + 1)));
            g.drawRect(x * stateWidth, y * stateHeight, stateWidth, stateHeight);
        }
    }
}

void Plate::resized()
{
}
