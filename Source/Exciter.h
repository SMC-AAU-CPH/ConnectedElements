/*
  ==============================================================================

    Exciter.h
    Created: 12 Dec 2018 10:52:55am
    Author:  Nikolaj Schwab Andersson

  ==============================================================================
*/

#pragma once
#include <vector>

using namespace std;

class StringExciter
{
public:
    StringExciter();

    void excite();
    void setLength(int L);
    void setLevel(double level);
    double getOutput();
    
    bool isPlaying() { return play; };
    
private:
  bool play = false;
  int pos = 0;
  float q = 1;
  int exciterLength = 10;
  int maxLength = 2000;
  double Fmax = 1.0f;

  vector<double> Fe;
};
