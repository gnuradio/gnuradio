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

#ifndef __SPHERICAL_H__
#define __SPHERICAL_H__

// wxWidgets includes
#include <wx/string.h>

class Spherical
{
public:
    Spherical();
    Spherical(double latitude, double longitude);
    Spherical(wxString latitude, wxString longitude);
    
    double Latitude() const { return m_latitude; }
    double Longitude() const { return m_longitude; }
    void SetLatitude(double latitude);
    void SetLongitude(double longitude);    
    
private:
    double m_latitude;
    double m_longitude;

    friend double range(const Spherical &from, const Spherical &to);
    friend double bearing(const Spherical &from, const Spherical &to);
};

#endif // __SPHERICAL_H__
