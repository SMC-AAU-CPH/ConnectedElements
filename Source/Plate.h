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

#include "../JuceLibraryCode/JuceHeader.h"

using namespace std;

class Plate : public Component
{
public:
    Plate(double fs);
    
    void setSampleRate (double sampleRate);
    void excite();
    float getOutput (float ratioX, float ratioY);
    
    void setImpactPosition (float xPos, float yPos);

    void setFrequency (float f);
    void setInput (float in) {input = in;};
    void setDamping (float frequencyDependent, float frequencyIndependent);
    
    tuple<int, int> getCP (int idx) { return cpIdx[idx]; };
    void setConnection (int idx, tuple<double, double> cp) {auto [x, y] = cp; cpIdx[idx] = make_tuple(floor(x * Nx), floor(y * Ny)); };
    int addConnection (tuple<double, double> cp);
    
    void addJFc (double JFc, tuple<int, int> cpIdx);
    
    int getNumXPoints() { return Nx; };
    int getNumYPoints() { return Ny; };
    
    int getS0() { return sigma0; };
    
    double getGridSpacing() { return sqrt(Nx / static_cast<double>(Ny)) / static_cast<double>(Nx); };
    
    double getPrevStateAt (tuple<int, int> cpIdx) {auto [x, y] = cpIdx; return un1[x][y]; };
    double getStateAt (tuple<int, int> cpIdx) {auto [x, y] = cpIdx; return un[x][y]; };
    double getNextStateAt (tuple<int, int> cpIdx) {auto [x, y] = cpIdx; return u[x][y]; };
    
    void updateUVectors();
    
    double clamp (double input, double min, double max);
    
    void paint(Graphics &g) override;
    
    void resized() override;
    
    void mouseDrag (const MouseEvent& e) override;
    
private:
    float frequency = 220;
    double k;
    double fs;
    const static int Nx = 20; // Number of spatial points
    const static int Ny = 10;
    const double h = getGridSpacing();
    
    double d, B1, B2, B3, C, C1, C2, C3, C4;
    double kappa, sigma0, sigma1;
    //vector<double> u, un, un1, excitationArea;
    double u[Nx][Ny] = {0};
    double un[Nx][Ny] = {0};
    double un1[Nx][Ny] = {0};
    double excitationArea[Nx][Ny] = {0};
    
    int strikePositionX = 5;
    int strikePositionY = 5;
    
    double input = 0.0;
    int outputPositionX = 2;
    int outputPositionY = 3;
    
    vector<tuple<int, int>> cpIdx;
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
