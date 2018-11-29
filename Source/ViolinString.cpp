/*
  ==============================================================================

    ViolinString.cpp
    Created: 14 Nov 2018 3:10:58pm
    Author:  Silvin Willemsen

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "ViolinString.h"

//==============================================================================
ViolinString::ViolinString (double freq, double fs) : fs (fs), freq (freq)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    setInterceptsMouseClicks (false, false);
    
    _bpX.store(0.25);
    _bpY.store(0);
    
    gamma = freq * 2;                       // Wave speed
    k = 1 / fs;                             // Time-step
    
    s0 = 1;                               // Frequency-independent damping
    s1 = 0.005;                             // Frequency-dependent damping

//    B = 0.0001;                             // Inharmonicity coefficient
//    kappa = sqrt (B) * (gamma / double_Pi); // Stiffness Factor
    kappa = 2;
    // Grid spacing
    h = sqrt ((gamma * gamma * k * k + 4.0 * s1 * k
              + sqrt (pow (gamma * gamma * k * k + 4.0 * s1 * k, 2.0)
                     + 16.0 * kappa * kappa * k * k)) * 0.5);
    
    N = floor (1.0 / h);                    // Number of gridpoints
    h = 1.0 / N;                            // Recalculate gridspacing

    // Initialise vectors
    u.resize (N);
    uPrev.resize (N);
    uNext.resize (N);
    
    reset();
    
    // Courant numbers
    lambdaSq = pow (gamma * k / h, 2);
    muSq = pow (k * kappa / (h * h), 2);

    kOh = (kappa * kappa) / (h * h);
    gOh = (gamma * gamma) / (h * h);
    // Bow Model
    a = 100;                                // Free parameter
    BM = sqrt(2 * a) * exp(0.5);

    
    _Vb.store(0.2);                               // Bowing speed
    _Fb.store(80);                                // Bowing force / total mass of bow;
    // Initialise variables for Newton Raphson
    tol = 1e-4;
    qPrev = 0;
}

void ViolinString::reset()
{
    fill(u.begin(), u.end(), 0.0);
    fill(uPrev.begin(), uPrev.end(), 0.0);
    fill(uNext.begin(), uNext.end(), 0.0);
}

//void ViolinString::setFrequency (double freq)
//{
//    gamma = freq * 2;                       // Wave speed
//   // we dont want to set the kappa according to the gamma 
////    kappa = sqrt (B) * (gamma / double_Pi); // Stiffness Factor
//    
//    // Grid spacing
//    h = sqrt ((gamma * gamma * k * k + 4.0 * s1 * k
//               + sqrt (pow (gamma * gamma * k * k + 4.0 * s1 * k, 2.0)
//                       + 16.0 * kappa * kappa * k * k)) * 0.5);
//
//    N = floor (1.0 / h);                    // Number of gridpoints
//    h = 1.0 / N;                            // Recalculate gridspacing
//    N = 100;
//    
//    // Courant numbers
//    lambdaSq = pow (gamma * k / h, 2);
//    muSq = pow (k * kappa / (h * h), 2);
//    
//    kOh = (kappa * kappa) / (h * h);
//    gOh = (gamma * gamma) / (h * h);
//}

ViolinString::~ViolinString()
{
}

void ViolinString::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.setColour (Colours::cyan);
    g.strokePath (generateStringPathAdvanced(), PathStrokeType (2.0f));
    g.setColour(Colours::orange);
    g.drawEllipse(floor(_cx.load() - 20), floor(_cy.load() - 5), 10, 10, 2);
    
    g.setColour(Colours::yellow);
    g.fillEllipse(fpx - 5, getHeight() / 2.0 - 5, 10, 10);
    
    // draw bow
    g.setColour (Colours::yellow);
    double opa = 90.0 / 100.0;
    if (opa >= 1.0)
    {
        g.setOpacity (1.0);
    } else {
        g.setOpacity(opa);
    }
    g.fillRect (floor(_bpX.load() * getWidth()), floor(_bpY.load() * getHeight()) - getHeight() / 2.0, 10, getHeight());
}

void ViolinString::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void ViolinString::bow()
{
    if(isnan(u[9]))
    {
        reset();
        return;
    }
    
    double Fb = _Fb.load();
    int bp = _bpX.load() * N;
    bool isBowing = _isBowing.load();
    
    newtonRaphson();
    double excitation = k * k * (1 / h) * Fb * BM * q * exp (-a * q * q);
    for (int l = 2; l < N - 2; ++l)
    {
        uNext[l] = (2 * u[l] - uPrev[l] + lambdaSq * (u[l+1] - 2 * u[l] + u[l-1])
                    - muSq * (u[l+2] - 4 * u[l+1] + 6 * u[l] - 4 * u[l-1] + u[l-2])
                    + s0 * k * uPrev[l]
                    + (2 * s1 * k) / (h * h) * ((u[l+1] - 2 * u[l] + u[l-1])
                                                - (uPrev[l+1] - 2 * uPrev[l] + uPrev[l-1]))) / (1 + s0 * k);
        if (l == bp && isBowing == true)
        {
            uNext[l] = uNext[l] - excitation;
        }
    }
    if (ff > 1)
    {
        ff = 1; std::cout << "wait" << std::endl;
    }
    uNext[fp] = uNext[fp] * (1 - ff);
}

void ViolinString::newtonRaphson()
{
    double Vb = _Vb.load();
    double Fb = _Fb.load();
    int bp = _bpX.load() * N;
    
    b = 2.0 / k * Vb - (2.0 / (k * k)) * (u[bp] - uPrev[bp])
    - gOh * (u[bp + 1] - 2 * u[bp] + u[bp - 1])
    + kOh * (u[bp + 2] - 4 * u[bp + 1] + 6 * u[bp] - 4 * u[bp - 1] + u[bp - 2])
    + 2 * s0 * Vb
    - (2 * s1 / (k * h * h)) * ((u[bp + 1] - 2 * u[bp] + u[bp - 1])
                                - (uPrev[bp + 1] - 2 * uPrev[bp] + uPrev[bp - 1]));
    eps = 1;
    int i = 0;
    while (eps>tol)
    {
        q=qPrev - (Fb * BM * qPrev * exp (-a * qPrev * qPrev) + 2 * qPrev / k + 2 * s0 * qPrev + b)
            / (Fb * BM * (1 - 2 * a * qPrev * qPrev) * exp (-a * qPrev * qPrev) + 2 / k + 2 * s0);
        eps = std::abs (q - qPrev);
        qPrev = q;
        ++i;
        if (i > 10000)
        {
            std::cout << "Nope" << std::endl;
        }
    }
}

void ViolinString::addJFc (double JFc, int index)
{
    uNext[index] = uNext[index] + JFc;
}

double ViolinString::getOutput (double ratio)
{
    int index = floor(ratio * N);
    return uNext[index];
}


void ViolinString::updateUVectors()
{
    uPrev = u;
    u = uNext;
}

void ViolinString::setRaisedCos (double exciterPos, double width)
{
    int j = 0;
    for (int i = floor(exciterPos*N) - floor(width / 2.0); i < floor(exciterPos*N) + floor(width / 2.0); ++i)
    {
        u[i] = (1 - cos(2 * double_Pi * j / width)) * 0.5;
        uPrev = u;
        ++j;
    }
    
}

void ViolinString::setRaisedCosSinglePoint (double exciterPos)
{
    u[floor(exciterPos * N)] = 1;
    uPrev = u;
}

Path ViolinString::generateStringPathAdvanced() 
{
    auto stringBounds = getHeight() / 2.0;
    Path stringPath;
    stringPath.startNewSubPath (0, stringBounds);
    
    auto spacing = getWidth() / double(N);
    auto x = spacing;
    
    for (int y = 0; y < N; y++)
    {
        const float newY = uNext[y] * 50000 + stringBounds;
        stringPath.lineTo(x, newY);
        if (y == _cpIdx.load())
        {
            _cx.store(x);
            _cy.store(newY);
        }
        if (y == fp)
        {
            fpx = x;
        }
        x += spacing;
    }
    stringPath.lineTo(getWidth(), stringBounds);
    
    return stringPath;
}

void ViolinString::mouseDrag(const MouseEvent &e)
{
}
