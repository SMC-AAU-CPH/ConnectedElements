/*
  ==============================================================================

    Exciter.cpp
    Created: 12 Dec 2018 10:52:55am
    Author:  Nikolaj Schwab Andersson

  ==============================================================================
*/
#include <math.h>
#include "Exciter.h"

StringExciter::StringExciter()
{
  Fe.resize(maxLength);

  for (int i = 0; i < exciterLength; i++)
    Fe[i] = Fmax / 2.0f * (1 - cos(2 * M_PI * i / exciterLength));
}

void StringExciter::excite()
{
  play = true;
}
void StringExciter::setLength(int L)
{
  if (L > maxLength)
    L = maxLength;

  exciterLength = L;

  for (int i = 0; i < exciterLength; i++)
    Fe[i] = Fmax * 0.5f * (1.0f - cos(q * M_PI * i / exciterLength));
}

void StringExciter::setLevel(double level)
{
  for (int i = 0; i < exciterLength; i++)
    Fe[i] = Fmax * 0.5f * (1.0f - cos(q * M_PI * i / exciterLength));
}

double StringExciter::getOutput()
{
  double output = 0.0f;

  if (play)
  {
    output = Fe[pos];
    pos++;
  }

  if (pos >= exciterLength)
  {
    pos = 0;
    play = false;
  }
  return output;
}