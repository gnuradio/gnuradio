/*
 Copyright 2006 Johnathan Corgan.
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 2
 as published by the Free Software Foundation.
 
 This software is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with GNU Radio; see the file COPYING.  If not, write to
 the Free Software Foundation, Inc., 51 Franklin Street,
 Boston, MA 02110-1301, USA.
*/

#ifndef __KNOWN_H__
#define __KNOWN_H__

// Application level includes
#include "spherical.h"
#include "sample.h"
#include "histogram.h"

// System level includes
#include <vector>

class KnownTransmitter
{
public:
    KnownTransmitter();

    void Location(const Spherical &location);
    Spherical Location() const { return m_location; }

    void Clear();
    void ClearStats();
    void Calc(const std::vector<Sample> &samples);

    int Count() const { return m_histogram.Count(); }
    int Mode() const { return m_histogram.Mode(); }
    float Mean() const { return m_histogram.Mean(); }
    float Concentration() const { return m_histogram.Concentration(); }

    bool   IsSet() const { return m_valid; }
    bool   HasStats() const { return m_histogram.Count() > 0; }
    double Latitude() const { return m_location.Latitude(); }
    double Longitude() const { return m_location.Longitude(); }
    const ErrorHistogram &Histogram() { return m_histogram; }
        
private:
    bool m_valid;
    Spherical m_location;
    ErrorHistogram m_histogram;
};

#endif
