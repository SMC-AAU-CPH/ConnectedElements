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
ViolinString::ViolinString(double freq, double fs, ObjectType stringType, int stringID, InstrumentType instrumentType) : fs(fs), freq(freq), stringType(stringType), stringID(stringID), instrumentType (instrumentType)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

    _bpX = 0.25;
    _bpY = 0;

    gamma = freq * 2; // Wave speed
    k = 1 / fs;       // Time-step

    s0 = 1;     // Frequency-independent damping
    s1 = 0.005; // Frequency-dependent damping

    //    B = 0.0001;                             // Inharmonicity coefficient
    //    kappa = sqrt (B) * (gamma / double_Pi); // Stiffness Factor
    kappa = 2;
    // Grid spacing
    
    if (stringType == pluckedString && instrumentType == dulcimer)
    {
        N = 28;
    }
    else if (instrumentType == hurdyGurdy)
    {
        if (stringType == bowedString)
            N = 78;
        else if (stringType == sympString)
            N = 28;
    } else {
        h = sqrt((gamma * gamma * k * k + 4.0 * s1 * k + sqrt(pow(gamma * gamma * k * k + 4.0 * s1 * k, 2.0) + 16.0 * kappa * kappa * k * k)) * 0.5);
        N = floor(1.0 / h); // Number of gridpoints
    }
    h = 1.0 / N; // Recalculate gridspacing

    // Initialise vectors

    uVecs.resize(3);
    for (int i = 0; i < 3; ++i)
        uVecs[i].resize(N);

    uNext = &uVecs[uNextPtrIdx][0];
    u = &uVecs[1][0];
    uPrev = &uVecs[2][0];

    reset();

    // Courant numbers
    lambdaSq = pow(gamma * k / h, 2);
    muSq = pow(k * kappa / (h * h), 2);

    kOh = (kappa * kappa) / (h * h);
    gOh = (gamma * gamma) / (h * h);
    // Bow Model
    a = 100; // Free parameter
    BM = sqrt(2 * a) * exp1(0.5);

    _Vb = 0.2; // Bowing speed
    _Fb = 80;  // Bowing force / total mass of bow;
    // Initialise variables for Newton Raphson
    tol = 1e-4;
    qPrev = 0;

    fp = 0;
    
    double B1 = s0 * k;
    double B2 = (2 * s1 * k) / (h * h);
    
    D = 1.0 / (1.0 + s0 * k);
    
    A1 = 2 - 2 * lambdaSq - 6 * muSq - 2 * B2;
    A2 = lambdaSq + 4 * muSq + B2;
    A3 = muSq;
    A4 = B1 - 1 + 2 * B2;
    A5 = B2;
    
    A1 *= D;
    A2 *= D;
    A3 *= D;
    A4 *= D;
    A5 *= D;
    
    E = k * k * (1 / h) * BM;
}

void ViolinString::reset()
{
    for (int i = 0; i < 3; ++i)
        fill(uVecs[i].begin(), uVecs[i].end(), 0.0);
}

ViolinString::~ViolinString()
{
}

void ViolinString::paint(Graphics &g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.setColour(stringType == bowedString ? Colours::cyan : (stringType == pluckedString ? Colours::mediumpurple : Colours::lawngreen));
    g.strokePath(generateStringPathAdvanced(), PathStrokeType(2.0f));
//    g.setColour(Colours::green);
//    for (int i = 1; i < N; ++i)
//    {
//        g.fillEllipse(i * getWidth() / double(N) - 3, getHeight() / 2 - 3, 6, 6);
//    }
    g.setColour(Colours::orange);
    for (int c = 0; c < cpIdx.size(); ++c)
    {
        g.drawEllipse(floor(cpIdx[c] * getWidth() / N - 5), floor(cy[c] - 5), 10, 10, 2);
    }

    if (stringType == bowedString)
    {
        g.setColour(Colours::yellow);
        g.fillEllipse(fp * getWidth() - 5, getHeight() / 2.0 - 5, 10, 10);

        // draw bow
        g.setColour(Colours::yellow);
        double opa = 90.0 / 100.0;
        if (opa >= 1.0)
        {
            g.setOpacity(1.0);
        }
        else
        {
            g.setOpacity(opa);
        }
        g.fillRect(floor(_bpX.load() * getWidth()), floor(_bpY.load() * getHeight()) - getHeight() / 2.0, 10, getHeight());
        g.setColour(Colour::greyLevel(0.5f).withAlpha(0.5f));
        for (double i = -12.0; i < 12.0; ++i)
        {
            double val = (1 - (pow(2.0, (i / 12.0)) - 1)) * getWidth() / 2.0;
            //            std::cout << val << std::endl;

            g.drawLine(val, 0, val, getHeight(), 2);
        }
        g.setColour(Colour::fromRGBA(255, 255, 0, 127));
        g.drawLine(getWidth() / 2.0, 0, getWidth() / 2.0, getHeight(), 2);
    }
    if (instrumentType == dulcimer && stringID % 2 == 1)
    {
        g.setColour(Colour::greyLevel(0.5f).withAlpha(0.5f));
        g.drawLine(0, getHeight() - 1, getWidth(), getHeight() - 1);
    }

}

void ViolinString::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
}

void ViolinString::bow()
{
    if (isnan(u[9]))
    {
        reset();
        return;
    }

    double Fb = _Fb.load();
    bowPos = clamp(_bpX.load() * N, 2, N - 3);
    int bp = floor(bowPos);
    bool isBowing = _isBowing;

    if (isBowing)
    {
        newtonRaphson();
    }

    //    if (pluck && pluckIdx < pluckLength)
    //    {
    //
    //    }
    double excitation = E * Fb * q * exp1(-a * q * q);
    for (int l = 2; l < N - 2; ++l)
    {
        uNext[l] = A1 * u[l] + A2 * (u[l + 1] + u[l - 1]) - A3 * (u[l + 2] + u[l - 2]) + A4 * uPrev[l] - A5 * (uPrev[l + 1] + uPrev[l - 1]);
    }

    if (_isPicking)
    {
        int width = exciterEnd - exciterStart;

        double in = stringExciter.getOutput();

        for (int i = exciterStart; i < exciterEnd; ++i)
        {
            //uNext[i] += ((1 - cos(1 * double_Pi * j / width)) * 0.5) * 0.001 * exciterForce;
            float distribution = (1 - cos(2 * double_Pi * i / width)) * 0.5;
            
            uNext[i] += distribution * in * exciterForce * 0.001;
        }
    }
    
    if (isBowing)
    {
        double alpha = bowPos - floor(bowPos);
//        if (t % 10000 == 0 && stringID == 0)
//            std::cout << alpha << " " << excitation * 100000 << std::endl;
//        ++t;
        if (interpolation == noStringInterpol)
        {
            uNext[bp] = uNext[bp] - excitation;
        }
        else if (interpolation == linear)
        {
            uNext[bp] = uNext[bp] - excitation * (1-alpha);

            if (bp < N - 3)
                uNext[bp + 1] = uNext[bp + 1] - excitation * alpha;
        }
        else if (interpolation == cubic)
        {
            if (bp > 3)
                uNext[bp - 1] = uNext[bp - 1] - excitation * (alpha * (alpha - 1) * (alpha - 2)) / -6.0;

            uNext[bp] = uNext[bp] - excitation * ((alpha - 1) * (alpha + 1) * (alpha - 2)) / 2.0;

            if (bp < N - 4)
                uNext[bp + 1] = uNext[bp + 1] - excitation * (alpha * (alpha + 1) * (alpha - 2)) / -2.0;
            if (bp < N - 5)
                uNext[bp + 2] = uNext[bp + 2] - excitation * (alpha * (alpha + 1) * (alpha - 1)) / 6.0;
        }
    }

    if (ff > 1)
    {
        ff = 1;
        std::cout << "wait" << std::endl;
    }
    
    if (stringType == bowedString)
    {
        ///////// heuristic interpolation
        int fingerPos = floor(fp * N - 1);
        double scale = 1;
        //pow((cos(double_Pi + (fp * N - 1 - fingerPos) * double_Pi) + 1) * 0.5, 4);
        double alphaFP = pow(fp * N - 1 - fingerPos, 6) * scale;
    //    if (t % 1000 == 0 && stringID == 0)
    //        std::cout << alphaFP << std::endl;
    //    ++t;
//        uNext[fingerPos - 2] = 0;
        uNext[fingerPos - 1] = 0;
        uNext[fingerPos] = uNext[fingerPos] * (alphaFP + (1-scale));
        uNext[fingerPos + 1] = uNext[fingerPos + 1] * (1 - alphaFP);
    }
}

void ViolinString::newtonRaphson()
{
    double Vb = _Vb.load();
    double Fb = _Fb.load();
    int bp = floor(bowPos);
    double alpha = bowPos - bp;
    
    if (interpolation == noStringInterpol)
    {
        uI = u[bp];
        uIPrev = uPrev[bp];
        uI1 = u[bp + 1];
        uI2 = u[bp + 2];
        uIM1 = u[bp - 1];
        uIM2 = u[bp - 2];
        uIPrev1 = uPrev[bp + 1];
        uIPrevM1 = uPrev[bp - 1];
    }
    else if (interpolation == linear)
    {
        uI = linearInterpolation(u, bp, alpha);
        uIPrev = linearInterpolation(uPrev, bp, alpha);
        uI1 = linearInterpolation(u, bp + 1, alpha);
        uI2 = linearInterpolation(u, bp + 2, alpha);
        uIM1 = linearInterpolation(u, bp - 1, alpha);
        uIM2 = linearInterpolation(u, bp - 2, alpha);
        uIPrev1 = linearInterpolation(uPrev, bp + 1, alpha);
        uIPrevM1 = linearInterpolation(uPrev, bp - 1, alpha);
    }
    else if (interpolation == cubic)
    {
        uI = cubicInterpolation(u, bp, alpha);
        uIPrev = cubicInterpolation(uPrev, bp, alpha);
        uI1 = cubicInterpolation(u, bp + 1, alpha);
        uI2 = cubicInterpolation(u, bp + 2, alpha);
        uIM1 = cubicInterpolation(u, bp - 1, alpha);
        uIM2 = cubicInterpolation(u, bp - 2, alpha);
        uIPrev1 = cubicInterpolation(uPrev, bp + 1, alpha);
        uIPrevM1 = cubicInterpolation(uPrev, bp - 1, alpha);
    }

    b = 2.0 / k * Vb - (2.0 / (k * k)) * (uI - uIPrev) - gOh * (uI1 - 2 * uI + uIM1) + kOh * (uI2 - 4 * uI1 + 6 * uI - 4 * uIM1 + uIM2) + 2 * s0 * Vb - (2 * s1 / (k * h * h)) * ((uI1 - 2 * uI + uIM1) - (uIPrev1 - 2 * uIPrev + uIPrevM1));
    eps = 1;
    int i = 0;
    while (eps > tol)
    {
        q = qPrev - (Fb * BM * qPrev * exp1(-a * qPrev * qPrev) + 2 * qPrev / k + 2 * s0 * qPrev + b) / (Fb * BM * (1 - 2 * a * qPrev * qPrev) * exp(-a * qPrev * qPrev) + 2 / k + 2 * s0);
        eps = std::abs(q - qPrev);
        qPrev = q;
        ++i;
        if (i > 10000)
        {
            std::cout << "Nope" << std::endl;
        }
    }
}

void ViolinString::addJFc(double JFc, int index)
{
    uNext[index] = uNext[index] + JFc;
}

double ViolinString::getOutput(double ratio)
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

void ViolinString::setRaisedCos(double exciterPos, double width, double force)
{

    exciterStart = clamp(exciterPos * N - width * 0.5, 2, N - 3);
    exciterEnd = clamp(exciterPos * N + width * 0.5, 2, N - 3);
    exciterForce = force;
}

Path ViolinString::generateStringPathAdvanced()
{
    auto stringBounds = getHeight() / 2.0;
    Path stringPath;
    stringPath.startNewSubPath(0, stringBounds);

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

void ViolinString::setConnection(int idx, double cp)
{
    if (cpIdx.size())
        cpIdx[idx] = clamp(floor(cp * N), 2, N - 2);
}

int ViolinString::addConnection(double cp)
{
    cpIdx.push_back(clamp(floor(cp * N), 2, N - 2));
    cx.push_back(0);
    cy.push_back(0);
    return static_cast<int>(cpIdx.size()) - 1;
}

double ViolinString::clamp(double input, double min, double max)
{
    if (input > max)
        return max;
    else if (input < min)
        return min;
    else
        return input;
}

void ViolinString::mouseDown(const MouseEvent &e)
{
    if (ModifierKeys::getCurrentModifiers() == ModifierKeys::leftButtonModifier && stringType == bowedString)
    {
        setBow(true);
    } else {
        setRaisedCos(e.x / static_cast<double>(getWidth()), 5, 0.05);
        pick(true);
    }
    if (e.y >= (getHeight() / 2.0) - cpMR && e.y <= (getHeight() / 2.0) + cpMR && ModifierKeys::getCurrentModifiers() == ModifierKeys::altModifier + ModifierKeys::leftButtonModifier)
    {
        bool cpMove = false;
        for (int i = 0; i < cpIdx.size(); ++i)
        {
            if (getWidth() * cpIdx[i] / static_cast<double>(N) >= e.x - cpMR && getWidth() * cpIdx[i] / static_cast<double>(N) <= e.x + cpMR)
            {
                cpMove = true;
                cpMoveIdx = i;
                break;
            }
        }
        // if the location of the click didn't contain an existing connection, create a new one
        if (!cpMove)
        {
            //create new connection
            std::cout << "check" << std::endl;
        }
    }
}

void ViolinString::mouseDrag(const MouseEvent &e)
{
    double maxVb = 0.2;

    if (cpMoveIdx != -1 || ModifierKeys::getCurrentModifiers() == ModifierKeys::altModifier + ModifierKeys::leftButtonModifier)
    {
        if (cpMoveIdx != -1)
        {
            double cp = e.x <= 0 ? 0 : (e.x < getWidth() ? e.x / static_cast<double>(getWidth()) : 1);
            cpIdx[cpMoveIdx] = floor(cp * N);
        }
    }
    else if (stringType == bowedString && ModifierKeys::getCurrentModifiers() == ModifierKeys::ctrlModifier + ModifierKeys::leftButtonModifier)
    {
        fp = e.x <= 0 ? 0 : (e.x < getWidth() ? e.x / static_cast<double>(getWidth()) : 1);
    }

    else if (cpMoveIdx == -1)
    {
        float bowVelocity = e.y / (static_cast<double>(getHeight())) * maxVb;
        setVb(bowVelocity);

        float bowPositionX = e.x <= 0 ? 0 : (e.x < getWidth() ? e.x / static_cast<double>(getWidth()) : 1);
        float bowPositionY = e.y <= 0 ? 0 : (e.y >= getHeight() ? 1 : e.y / static_cast<double>(getHeight()));

        _bpX.store(bowPositionX);
        _bpY.store(bowPositionY);
    }
}

void ViolinString::mouseUp(const MouseEvent &e)
{
    setBow(false);
    cpMoveIdx = -1;
}

double ViolinString::linearInterpolation(double* uVec, int bp, double alpha)
{
    return uVec[bp] * (1 - alpha) + uVec[bp + 1] * alpha;
}

double ViolinString::cubicInterpolation(double* uVec, int bp, double alpha)
{
    return uVec[bp - 1] * (alpha * (alpha - 1) * (alpha - 2)) / -6.0
            + uVec[bp] * ((alpha - 1) * (alpha + 1) * (alpha - 2)) / 2.0
            + uVec[bp + 1] * (alpha * (alpha + 1) * (alpha - 2)) / -2.0
            + uVec[bp + 2] * (alpha * (alpha + 1) * (alpha - 1)) / 6.0;
}
