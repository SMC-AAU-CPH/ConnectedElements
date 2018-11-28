/*
  ==============================================================================

    Plate.h
    Created: 1 Oct 2018 1:20:08pm
    Author:  Nikolaj Schwab Andersson
    
    Plate
    - damping [0 -> 0.5], watch out for instability
    - frequency (not really precise frequency here yet) 
    - strike position and area 
    - pickup position

  ==============================================================================
*/

#pragma once

#include <math.h>
#include <iostream>
#include "../JuceLibraryCode/JuceHeader.h"

using namespace std;

class Plate : public Component
{
public:
  Plate()
  {
    //setSampleRate(1.0f / sample_duration);
    sigma0 = 0.1;
    sigma1 = 0.005;
  }

  void setSampleRate(double sampleRate)
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

  float getOutput(double input)
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

  void setImpactPosition(float xPos, float yPos)
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

  void setFrequency(float f)
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

  void setDamping(float frequencyDependent, float frequencyIndependent)
  {
    sigma1 = clamp(frequencyDependent, 0.0001, 0.5);    // Frequency dependent damping
    sigma0 = clamp(frequencyIndependent, 0.0001, 10.0); // Frequency independent damping
  }

  double clamp(double in, double min, double max)
  {
    if (in > max)
      return max;
    else if (in < min)
      return min;
    else
      return in;
  }

  void paint(Graphics &g)
  {
    //    std::cout << "painting" << std::endl;
  }

  void resized()
  {
    //std::cout << "resized" << std::endl;
  }

private:
  float frequency = 220;
  double k = 1.0f / 48000.0f;
  double fs = 48000.0f;
  const static int Nx = 10; // Number of spatial points
  const static int Ny = 10;
  const double h = double(1.0f / Nx);

  double d, B1, B2, B3, C, C1, C2, C3, C4;
  double kappa, sigma0, sigma1;
  //vector<double> u, un, un1, excitationArea;
  double u[Nx][Ny] = {0};
  double un[Nx][Ny] = {0};
  double un1[Nx][Ny] = {0};
  double excitationArea[Nx][Ny] = {0};

  int strikePositionX = 5;
  int strikePositionY = 5;

  int outputPositionX = 5;
  int outputPositionY = 5;
};
class PlateExciter
{
public:
  PlateExciter()
  {
    Fe.resize(maxLength);

    for (int i = 0; i < exciterLength; i++)
      Fe[i] = Fmax / 2.0f * (1 - cos(2 * M_PI * i / exciterLength));

    /*for (int i = 0; i < exciterLength; i++)
         std::cout << "exciter: " << Fe[i] << "\n";*/
  }

  void excite()
  {
    play = true;
  }
  void setLength(int L)
  {
    if (L > maxLength)
      L = maxLength;

    exciterLength = L;

    for (int i = 0; i < exciterLength; i++)
      Fe[i] = Fmax * 0.5f * (1.0f - cos(q * M_PI * i / exciterLength));
  }

  void setLevel(double level)
  {
    //Fmax = level * 1e8;

    for (int i = 0; i < exciterLength; i++)
      Fe[i] = Fmax * 0.5f * (1.0f - cos(q * M_PI * i / exciterLength));
  }

  double getOutput()
  {
    double output = 0.0f;

    if (play)
    {
      output = Fe[pos];
      //std::cout << "exciter: " << Fe[pos] << "\n";
      pos++;
    }

    if (pos >= exciterLength)
    {
      pos = 0;
      play = false;
    }
    return output;
  }

  bool play = false;
  int pos = 0;
  float q = 1;
  int exciterLength = 10;
  int maxLength = 2000;
  double Fmax = 1.0f;

  vector<double> Fe;
};
