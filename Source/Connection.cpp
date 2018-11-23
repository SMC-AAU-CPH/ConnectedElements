/*
  ==============================================================================

    Connection.cpp
    Created: 22 Nov 2018 6:16:29pm
    Author:  Nikolaj Schwab Andersson

  ==============================================================================
*/

#include "Connection.h"
Connection::Connection(ViolinString* object1, ViolinString* object2,
                       double cp1, double cp2,
                       double width1, double width2,
                       double sx, double w0, double w1) : cp1(cp1), cp2(cp2),
                                                          width1(width1), width2(width2),
                                                          sx(sx), w0(w0), w1(w1)
{
    objects.add(object1);
    objects.add(object2);
}


