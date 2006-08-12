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

#ifndef __GPS_H__
#define __GPS_H__

// Autoconf generated configure options
#if HAVE_CONFIG_H
    #include "config.h"
#endif

// Application level includes
#include "spherical.h"

// wxWidgets includes
#include <wx/event.h>

// System level includes
#include <math.h>  // For some reason, <cmath> doesn't include modf needed in gps.cpp

class GPRMC;

class GPSUpdate : public wxNotifyEvent
{
public:
    GPSUpdate(const wxEventType &event, GPRMC *gprmc);
    virtual wxEvent *Clone() const { return new GPSUpdate(*this); }
    GPRMC *m_gprmc;
};

extern const wxEventType wxEVT_GPS_UPDATE;

typedef void(wxEvtHandler::*GPSUpdateFunction)(GPSUpdate&);

#define EVT_GPS_UPDATE(fn) \
        DECLARE_EVENT_TABLE_ENTRY( \
            wxEVT_GPS_UPDATE, -1, -1, \
            (wxObjectEventFunction)(wxEventFunction)(GPSUpdateFunction)&fn, \
            (wxObject *)NULL \
        ),

class NMEA
{
public:
    static bool Checksum(char *sentence);
    static char *Field(char *sentence, int num);
    static double Coord(char *sentence, int num);
};

class GPRMC : public NMEA
{
public:
    GPRMC(char *sentence);
    void AsString(char *buf);
        
    time_t m_stamp;
    bool   m_valid;
    Spherical m_fix;
    float m_speed;
    float m_heading;
    float m_magnetic;
    unsigned char m_mode;
};

class GPS;
class SerialPort;

class GPSBackground : public wxThread
{
public:
    GPSBackground(GPS *gps);
    virtual ExitCode Entry();
    bool IsRunning() { return m_running; }

private:
    void PerLoop();

    bool m_running;
    GPS *m_gps;
    SerialPort *m_port;
};

class wxString;

class GPS
{
public:
    GPS(wxEvtHandler *dest);
    ~GPS();
    
    bool Start(wxString &port);
    bool Stop();
    SerialPort *GetPort() { return m_port; }
    void RxData(char *buffer);
    
private:
    void RxGPRMC(char *buffer);

    GPSBackground *m_thread;
    wxEvtHandler *m_dest;
    SerialPort *m_port;
};

#endif // __GPS_H__
