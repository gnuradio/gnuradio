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
#include "sample.h"
#include "util.h"

// wxWidgets includes
#include <wx/log.h>

Sample::Sample()
{
    m_time = 0;
    m_valid = false;
    m_location = Spherical(0.0, 0.0);
    m_heading = 0.0;
    m_speed = 0.0;
    m_in_phase = 0.0;
    m_quadrature = 0.0;
    m_phase = 0.0;
    m_strength = 0.0;
    m_volume = 0.0;
    m_rate = 0;
    m_filtering = 1;
    m_error = 0.0;
    m_ierror = 0.0;
    m_qerror = 0.0;
}

Sample::Sample(const Sample &sample)
{
    m_time = sample.m_time;
    m_valid = sample.m_valid;
    m_location = sample.m_location;
    m_heading = sample.m_heading;
    m_speed = sample.m_speed;
    m_in_phase = sample.m_in_phase;
    m_quadrature = sample.m_quadrature;
    m_phase = sample.m_phase;
    m_strength = sample.m_strength;
    m_volume = sample.m_volume;
    m_rate = sample.m_rate;
    m_filtering = sample.m_filtering;
    m_error = sample.m_error;
    m_ierror = sample.m_ierror;
    m_qerror = sample.m_qerror;
}

Sample::Sample(wxString &line)
{
    char valid;
    double lat, lon;

    sscanf((char *)line.c_str(), "%c %i %lf %lf %f %f %f %f %f %f %f %f %f %f %i %i",
        &valid,
        &m_time,
        &lat,
        &lon,
        &m_heading,
        &m_speed,
        &m_volume,
        &m_strength,
        &m_in_phase,
        &m_quadrature,
        &m_phase,
        &m_error,
        &m_ierror,
        &m_qerror,
        &m_rate,
        &m_filtering);    

    if (valid == 'V') 
        m_valid = true;
    else
        m_valid = false;

    m_location = Spherical(lat, lon);
}

void Sample::Dump(char *str, int len)
{
    //                 vld tim  lat     lon     hdg   speed vol   stren inphs quad  phase error ierr  qerr  rt flt
    snprintf(str, len, "%s %10i %10.5lf %10.5lf %5.1f %6.2f %7.5f %7.5f %9.6f %9.6f %9.6f %7.2f %9.6f %9.6f %i %i",
                    m_valid ? "V":"I",
                    m_time,
                    m_location.Latitude(),
                    m_location.Longitude(),
                    m_heading,
                    m_speed,
                    m_volume,
                    m_strength,
                    m_in_phase,
                    m_quadrature,
                    m_phase,
                    m_error,
                    m_ierror,
                    m_qerror,
                    m_rate,
                    m_filtering);    
}

void Sample::CalcError(const Spherical &location, float &angle, float &ierror, float &qerror) const
{
    float actual_bearing = bearing(m_location, location);
    float sample_relative_bearing = degree_normalize(to_degrees(m_phase));
    float sample_absolute_bearing = degree_normalize(sample_relative_bearing+m_heading);
    angle = sample_absolute_bearing-actual_bearing;
    
    if (angle < -180.0)
        angle += 360;
    if (angle > 180.0)
        angle -= 360;

    ierror = 0.0;
    qerror = 0.0;

    // Rotate I, Q by actual bearing
    float i_act = cos(to_radians(-actual_bearing));
    float q_act = sin(to_radians(-actual_bearing));
    ierror = m_in_phase*i_act - m_quadrature*q_act;
    qerror = m_quadrature*i_act + m_in_phase*q_act;


}
