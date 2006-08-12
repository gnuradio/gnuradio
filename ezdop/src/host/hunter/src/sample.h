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

#ifndef __SAMPLE_H__
#define __SAMPLE_H__

// Application level includes
#include "spherical.h"

// wxWidgets includes
#include <wx/string.h>

// System level includes
#include <ctime>

class Sample
{
public:
    Sample();
    Sample(const Sample &sample);
    Sample(wxString &line);

    void Time(time_t time)             { m_time = time; }
    void Valid(bool valid)             { m_valid = valid; }
    void Location(Spherical &location) { m_location = location; }
    void Heading(float heading)        { m_heading = heading; }
    void Speed(float speed)            { m_speed = speed; }
    void InPhase(float in_phase)       { m_in_phase = in_phase; }
    void Quadrature(float quadrature)  { m_quadrature = quadrature; }
    void Phase(float phase)            { m_phase = phase; }
    void Strength(float strength)      { m_strength = strength; }
    void Volume(float volume)          { m_volume = volume; }
    void Rate(int rate)                { m_rate = rate; }
    void Filtering(int filtering)      { m_filtering = filtering; }
    void Error(float error)            { m_error = error; }
    void IError(float error)           { m_ierror = error; }
    void QError(float error)           { m_qerror = error; }
                
    const Spherical &Location() const { return m_location; }
    float Latitude() const { return m_location.Latitude(); }
    float Longitude() const { return m_location.Longitude(); }
    float Heading() const { return m_heading; } 
    float Speed() const { return m_speed; }
    float InPhase() const { return m_in_phase; }
    float Quadrature() const { return m_quadrature; }
    float Phase() const { return m_phase; }
    float Strength() const { return m_strength; }
    float Volume() const { return m_volume; }
    int   Filtering() const { return m_filtering; }
    float Error() const { return m_error; }
    float IError() const { return m_ierror; }
    float QError() const { return m_qerror; }
    bool  Valid() const { return m_valid; }
    
    void CalcError(const Spherical &location, float &error, float &ierror, float &qerror) const;
    
    void Dump(char *str, int len); // TEMPORARY
    
    operator const std::string();    // Conversion operator to std::string
            
private:
    // Data supplied by measuring system
    time_t    m_time;           // Unix time of observation
    bool      m_valid;          // GPS validity indication (NMEA "I" or "V")
    Spherical m_location;       // GPS latitude and longitude
    float     m_heading;        // GPS heading in degrees 0.0 - 360.0
    float     m_speed;          // GPS speed in mph
    float     m_in_phase;       // Doppler I channel -1.0 to 1.0
    float     m_quadrature;     // Doppler Q channel -1.0 to 1.0
    float     m_phase;          // Doppler phase -M_PI to M_PI (derived)
    float     m_strength;       // Doppler strength 0.0 - 1.0 (derived)
    float     m_volume;         // Doppler volume 0.0 - 1.0
    int       m_rate;           // Doppler rotation rate 0 - 5
    int       m_filtering;      // Doppler filtering 1 - 100

    // Container configured
    float     m_error;          // Known transmitter bearing error
    float     m_ierror;         // Known transmitter in phase error
    float     m_qerror;         // Known transmitter quadrature error
};

#endif
