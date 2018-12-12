/*
    Diode-Clipper 
    by Nikolaj Andersson, 6. June 2018
*/
#include <math.h>
#pragma once

class Clipper
{
  public:
    void setSamplingRate(double samplingRate)
    {
        fs = samplingRate;
        T = 1.0f / samplingRate;
    }
    void reset()
    {
        Vin = 0;
        dVin = 0;

        Vo = 0;
        dVo = 0;

        f = 0;
        df = 0;
        step = 0;
    }

    float process(float input)
    {
        Vin = input;
        float lambda = 1;

        for (int i = 0; i < iterations; i++)
        {
            f = 0.5 * ((Vin - Vo) * coef1 - coef2 * sinh(Vo / Vt)) + 0.5 * ((dVin - dVo) * coef1 - coef2 * sinh(dVo / Vt)) - (1 / T) * (Vo - dVo);

            df = 0.5 * (-1 / (R * C) - 2 * Is / (C * Vt) * cosh(Vo / Vt)) - (1 / T);

            step = f / df;

            step = clamp(step, -stepsize, stepsize);

            double newVo = Vo - lambda * step;

            double newF = 0.5 * ((Vin - newVo) * coef1 - coef2 * sinh(newVo / Vt)) + 0.5 * ((dVin - dVo) * coef1 - coef2 * sinh(dVo / Vt)) - (1 / T) * (newVo - dVo);

            if (fabs(newF) >= fabs(f))
                lambda = lambda * 0.5;

            if (fabs(Vo - newVo) < err)
                break;

            Vo = newVo;
        }

        dVin = Vin;
        dVo = Vo;

        return Vo;
    }

  private:
    double clamp(double input, double min, double max)
    {
        if (input > max)
            return max;
        else if (input < min)
            return min;
        else
            return input;
    }
    double T = 0;
    double fs = 44100;

    double Vin = 0;
    double dVin = 0;

    double Vo = 0;
    double dVo = 0;

    const double Is = 2.52e-9;
    const double Vt = 45.3e-3;

    const float R = 2200;
    const double C = 10e-9;

    double f = 0;
    double df = 0;
    double step = 0;

    const double coef1 = 1.0f / (R * C);
    const double coef2 = 2.0f * Is / C;
    //const float coef3 = coef2 * Vt;

    const double err = 1e-7;
    const int iterations = 10;
    const float stepsize = 0.5;
};
