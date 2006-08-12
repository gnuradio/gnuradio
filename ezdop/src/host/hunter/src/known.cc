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
 the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 Boston, MA 02111-1307, USA.
*/

// Application level includes
#include "known.h"
#include "util.h"

// wxWidget includes
#include <wx/log.h>

using namespace std;

KnownTransmitter::KnownTransmitter()
{
    m_valid = false;
    ClearStats();
}

void KnownTransmitter::Location(const Spherical &location)
{
    m_location = location;
    m_valid = true;
    ClearStats();
}

void KnownTransmitter::Clear()
{
    m_valid = false;
    ClearStats();
}

void KnownTransmitter::ClearStats()
{
    m_histogram.Reset();
}

void KnownTransmitter::Calc(const std::vector<Sample> &samples)
{
    m_histogram.Calc(samples, m_location);
}
