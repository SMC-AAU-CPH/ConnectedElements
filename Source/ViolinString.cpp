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
//    setInterceptsMouseClicks (false, false);
    
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
    
    uVecs.resize(3);
    for (int i = 0; i < 3; ++i)
        uVecs[i].resize (N);
    
    uNext = &uVecs[uNextPtrIdx][0];
    u = &uVecs[1][0];
    uPrev = &uVecs[2][0];
    
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
    
    fp = 0;
}

void ViolinString::reset()
{
    for (int i = 0; i < 3; ++i)
        fill(uVecs[i].begin(), uVecs[i].end(), 0.0);
}

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
    for (int c = 0; c < cpIdx.size(); ++c)
    {
        g.drawEllipse(floor(cx[c] - 20), floor(cy[c] - 5), 10, 10, 2);
    }
    
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
    double bowPos = clamp(_bpX.load() * N, 2, N - 3);
    int bp = floor(bowPos);
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
    }
    
    if (isBowing)
    {
        double alpha = bowPos - floor(bowPos);
        if (interpolation == noInterpolation)
        {
            uNext[bp] = uNext[bp] - excitation;
        }
        else if (interpolation == linear)
        {
            uNext[bp] = uNext[bp] - excitation * (1 - alpha);
            
            if (bp < N - 3)
                uNext[bp + 1] = uNext[bp + 1] - excitation * alpha;
        }
        else if (interpolation == cubic)
        {
            if (bp > 3)
                uNext[bp-1] = uNext[bp-1] - excitation * (alpha * (alpha - 1) * (alpha - 2)) / -6.0;
            
            uNext[bp] = uNext[bp] - excitation * ((alpha - 1) * (alpha + 1) * (alpha - 2)) / 2.0;
            
            if (bp < N - 4)
                uNext[bp+1] = uNext[bp+1] - excitation * (alpha * (alpha + 1) * (alpha - 2)) / -2.0;
            if (bp < N - 5)
                uNext[bp+2] = uNext[bp+2] - excitation * (alpha * (alpha + 1) * (alpha - 1)) / 6.0;
        }
    }
    
    if (ff > 1)
    {
        ff = 1; std::cout << "wait" << std::endl;
    }
    int fingerPos = floor(fp * N);
    uNext[fingerPos] = uNext[fingerPos] * (1 - ff);
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
    uNext = &uVecs[uNextPtrIdx][0];
    uNextPtrIdx = (uNextPtrIdx + 1) % 3;
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
//    u[floor(exciterPos * N)] = 1;
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
        for (int c = 0; c < cpIdx.size(); ++c)
        {
            if (y == cpIdx[c])
            {
                cx[c] = x;
                cy[c] = newY;
            }
        }
        if (y == floor(fp * N))
        {
            fpx = x;
        }
        x += spacing;
    }
    stringPath.lineTo(getWidth(), stringBounds);
    
    return stringPath;
}

int ViolinString::addConnection (double cp)
{
    cpIdx.push_back (clamp(floor(cp * N), 2, N-2));
    cx.push_back(0);
    cy.push_back(0);
    return static_cast<int>(cpIdx.size()) - 1;
}

double ViolinString::clamp (double input, double min, double max)
{
    if (input > max)
        return max;
    else if (input < min)
        return min;
    else
        return input;
}

void ViolinString::mouseDown(const MouseEvent& e)
{
    if (ModifierKeys::getCurrentModifiers() == ModifierKeys::leftButtonModifier)
    {
        setBow(true);
    }
}

void ViolinString::mouseDrag (const MouseEvent& e)
{
    double maxVb = 0.2;
    
    if (ModifierKeys::getCurrentModifiers() == ModifierKeys::altModifier + ModifierKeys::leftButtonModifier)
    {
        double cp = e.x <= 0 ? 0 : (e.x < getWidth() ? e.x / static_cast<double>(getWidth()) : 1);
        cpIdx[0] = floor(cp * N);
    }
    else if (ModifierKeys::getCurrentModifiers() == ModifierKeys::altModifier + ModifierKeys::rightButtonModifier)
    {
        double cp = e.x <= 0 ? 0 : (e.x < getWidth() ? e.x / static_cast<double>(getWidth()) : 1);
        cpIdx[1] = floor(cp * N);
    }
    else if (ModifierKeys::getCurrentModifiers() == ModifierKeys::ctrlModifier + ModifierKeys::leftButtonModifier)
    {
        fp = e.x <= 0 ? 0 : (e.x < getWidth() ? e.x / static_cast<double>(getWidth()) : 1);
    }
    
    else
    {
        float bowVelocity = e.y / (static_cast<double>(getHeight())) * maxVb;
        setVb(bowVelocity);
        
        float bowPositionX = e.x <= 0 ? 0 : (e.x < getWidth() ? e.x / static_cast<double>(getWidth()) : 1);
        float bowPositionY = e.y <= 0 ? 0 : (e.y >= getHeight() ? 1 : e.y / static_cast<double>(getHeight()));
        
        _bpX.store (bowPositionX);
        _bpY.store (bowPositionY);
    }
}

void ViolinString::mouseUp(const MouseEvent &e)
{
    setBow(false);
}
