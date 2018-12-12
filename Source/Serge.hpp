/*
Serge Wave Multiplier
by Nikolaj Andersson, 10. August 2018
*/
#include <math.h>
#pragma once

class SergeWavefolder
{
  public:
    void setSamplingRate(double samplingRate)
    {
        Fs = samplingRate;
        T = 1.0f / samplingRate;
    }
    void reset()
    {
        vin = 0;
        dVin = 0;
    }

    double process(double input)
    {
        vin = input;
        double out, W, lambda, dlambda;

        if (fabs(vin - dVin) < 1e-8)
        {
            // ill condition
            double avgVin = (vin + dVin) * 0.5;
            lambda = sgn(avgVin);

            double e1 = (lambda * avgVin) / (n * Vt);
            double c1 = (R1 * Is) / (n * Vt);

            // Lambert W solver using Fritsch iteration
            W = LW(c1 * exp(e1));

            out = avgVin - 2 * lambda * n * Vt * W;
        }
        else
        {
            // anti-aliasing
            lambda = sgn(vin);

            double e1 = (lambda * vin) / (n * Vt);
            double c1 = (R1 * Is) / (n * Vt);
            double c2 = (n * Vt) * (n * Vt);

            double W1 = LW(c1 * exp(e1));
            double W2 = LW(c1 * exp(e1)) * (W1 + 2);

            double Fn = (vin * vin) / 2 - c2 * W2;

            dlambda = sgn(dVin);
            double e2 = (dlambda * dVin) / (n * Vt);

            double dW1 = LW(c1 * exp(e2));
            double dW2 = LW(c1 * exp(e2)) * (dW1 + 2);

            double Fn1 = (dVin * dVin) / 2 - c2 * dW2;
            double diff = vin - dVin;

            out = (Fn - Fn1) / diff;
        }

        dVin = vin;

        return out;
    }
    
  private:
    double sgn(double x){
        if (x > 0) return 1;
        if (x < 0) return -1;
        return 0;
    }
    
    
    double T = 0;
    double Fs = 44100;

    double vin = 0;
    double dVin = 0;

    double Is = 2.52e-9;
    double n = 1.752;
    double R1 = 33e3;
    double Vt = 25.864e-3;

    double LW(const double x)
    {
        /*
    This function evaluates the upper branch of the Lambert-W function for
    real input x. Function is not optimized for vectorial processing.

    Function written by F. Esqueda 2/10/17 based on implementation presented 
    by Darko Veberic - "Lambert W Function for Applications in Physics"
    Available at https://arxiv.org/pdf/1209.0735.pdf

    Ported to C++ by Nikolaj Andersson
    */

        double thresh = 10e-12;
        double w;

        if (x < double(0.14546954290661823))
        {
            double num = 1 + 5.931375839364438 * x + 11.39220550532913 * (x * x) + 7.33888339911111 * (x * x * x) + 0.653449016991959 * (x * x * x * x);
            double den = 1 + 6.931373689597704 * x + 16.82349461388016 * (x * x) + 16.43072324143226 * (x * x * x) + 5.115235195211697 * (x * x * x * x);
            w = x * num / den;
        }
        else if (x < double(8.706658967856612))
        {
            double num = 1 + 2.4450530707265568 * x + 1.3436642259582265 * (x * x) + 0.14844005539759195 * (x * x * x) + 0.0008047501729130 * (x * x * x * x);
            double den = 1 + 3.4447089864860025 * x + 3.2924898573719523 * (x * x) + 0.9164600188031222 * (x * x * x) + 0.05306864044833221 * (x * x * x * x);
            w = x * num / den;
        }
        else
        {
            double a = log(x);
            double b = log(a);
            double ia = 1 / a;
            w = a - b + (b * ia) * 0.5 * b * (b - 2) * (ia * ia) + (1 / 6) * (2 * b * b - 9 * b + 6) * (ia * ia * ia);
        }

        double w1, z, q, e;

        for (int m = 0; m < 20; m++)
        {
            w1 = w + 1;
            z = log(x) - log(w) - w;
            q = 2 * w1 * (w1 + (2 / 3) * z);
            e = (z / w1) * ((q - z) / (q - 2 * z));
            w = w * (1 + e);

            if (fabs(e) < thresh)
                break;
        }

        return w;
    }
};
