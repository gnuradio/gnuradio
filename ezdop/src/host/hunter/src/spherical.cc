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

// Application level includes
#include "spherical.h"
#include "util.h"

Spherical::Spherical()
{
    m_latitude = 0.0;
    m_longitude = 0.0;
}

Spherical::Spherical(double latitude, double longitude)
{
    m_latitude = latitude;
    m_longitude = longitude;
}

Spherical::Spherical(wxString latitude, wxString longitude)
{
    latitude.ToDouble(&m_latitude);
    longitude.ToDouble(&m_longitude);
}

void Spherical::SetLatitude(double latitude)
{
    m_latitude = latitude;  // TODO: error handle
}

void Spherical::SetLongitude(double longitude)
{
    m_longitude = longitude;
}

double range(const Spherical &from, const Spherical &to)
{
    double lat1 = to_radians(from.m_latitude);
    double lon1 = to_radians(from.m_longitude);

    double lat2 = to_radians(to.m_latitude);
    double lon2 = to_radians(to.m_longitude);
    
    double n1 = sin((lat1-lat2)/2);
    double n2 = sin((lon1-lon2)/2);   
    double distance = 2*asin(sqrt(n1*n1 + cos(lat1)*cos(lat2)*n2*n2));

    return to_degrees(distance)*60.0*1.15077945; // Conversion to statute miles

}

double bearing(const Spherical &from, const Spherical &to)
{
    double lat1 = to_radians(from.m_latitude);
    double lon1 = to_radians(from.m_longitude);
    double lat2 = to_radians(to.m_latitude);
    double lon2 = to_radians(to.m_longitude);

    double bearing = atan2(-sin(lon1-lon2)*cos(lat2), cos(lat1)*sin(lat2)-sin(lat1)*cos(lat2)*cos(lon1-lon2));
    return degree_normalize(to_degrees(bearing));
}
