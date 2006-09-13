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
#include "gps.h"
#include "serial.h"

// wxWidgets includes
#include <wx/log.h>

#define NMEA_BAUD       4800
#define NMEA_BUFSIZE    82      // Maximum NMEA sentence length

const wxEventType wxEVT_GPS_UPDATE = wxNewEventType();

GPSUpdate::GPSUpdate(const wxEventType &event, GPRMC *gprmc) :
wxNotifyEvent(event)
{
    m_gprmc = gprmc;
}

GPSBackground::GPSBackground(GPS *gps)
{
    wxLogDebug(_T("GPSBackground::GPSBackground()"));
    wxASSERT(gps);
    
    m_running = false;
    m_gps = gps;
    m_port = gps->GetPort();
	Create();
}

// It's in thread.h but somehow gets undef'd
typedef void *ExitCode;
ExitCode GPSBackground::Entry()
{
    wxLogDebug(_T("GPSBackground::GPSBackground: entry"));

    m_running = true;
	while (!TestDestroy()) 
        PerLoop();
	m_running = false;

    wxLogDebug(_T("GPSBackground::GPSBackground: exit"));
}

void GPSBackground::PerLoop()
{
    static char buffer[NMEA_BUFSIZE];
    static int offset = 0;

    while(m_port->RxReady() > 0) {
        while (offset < NMEA_BUFSIZE) {
            // Read a byte into the buffer from the GPS data
            if (m_port->Read(&buffer[offset], 1) != 1)
                return;  // No more to read or read error/timeout, bail

            // Test for end of NMEA message
            if (buffer[offset] == '\r' || buffer[offset] == '\n') {
                buffer[offset] = '\0'; // Append end of string null
                if (strlen(buffer))
                    m_gps->RxData(buffer);
                offset = 0;
            }
            else
                offset++;
        }
        
        wxLogDebug(_T("GPSBackground: discarding too long input"));
        offset = 0;
    }        

    wxMilliSleep(500);
}

GPS::GPS(wxEvtHandler *dest)
{
    wxLogDebug(_T("GPS::GPS()"));
    m_thread = NULL;
    m_dest = dest;
}

GPS::~GPS()
{
    wxLogDebug(_T("GPS::~GPS()"));
}

bool GPS::Start(wxString &port)
{
    wxLogDebug(_T("GPS::Start(): %s"), port.c_str());
    m_port = new SerialPort(port);

    if (m_port->Open(NMEA_BAUD) == false) {
        delete m_port;
        return false;
    }   

    m_thread = new GPSBackground(this);
    m_thread->Run();    
    return true;
}

bool GPS::Stop()
{
    wxLogDebug(_T("GPS::Stop()"));

    if (m_thread && m_thread->IsRunning()) {
        m_thread->Delete();
        while (m_thread->IsRunning()) {
            wxYieldIfNeeded();
        }
    }
    
    m_thread = NULL;

    m_port->Close();
    if (m_port)
        delete m_port;

    return true;
}

bool NMEA::Checksum(char *sentence)
{
    unsigned char checksum = '\0';
    char ch, *pos = sentence, ctxt[3];
    
    while ((ch = *pos++) != '*' && ch != '\0')
        checksum ^= ch;
        
    sprintf(ctxt, "%02X", checksum);
    if (strncmp(ctxt, pos, 2))
        return false;
    else
        return true;
}

char *NMEA::Field(char *sentence, int num)
{
    static char result[NMEA_BUFSIZE];
    char ch, *pos = sentence;
            
    while (num-- > 0)
        while ((ch = *pos++) != ',' && ch != '\0')
            continue;

    strncpy(result, pos, NMEA_BUFSIZE-1);
    int i = 0;
    pos = result;
    while (*pos && *pos != ',' && *pos != '*' && *pos != '\r' && ++i < NMEA_BUFSIZE)
        pos++;
    
    *pos = 0;
    return result;
}

double NMEA::Coord(char *sentence, int num)
{
    double coord, degrees, minutes;

    sscanf(Field(sentence, num), "%lf", &coord);
    minutes = 100.0*modf(coord/100.0, &degrees);
    coord = degrees+minutes/60.0;

    char *ptr = Field(sentence, num+1);
    if (*ptr == 'S' || *ptr == 'W')
        coord = -coord;

    return coord;
}

void GPS::RxData(char *buffer)
{
    wxASSERT(buffer);
    
    if (NMEA::Checksum(buffer+1)) {
        if (strncmp("$GPRMC", buffer, 6) == 0) {
            GPRMC *fix = new GPRMC(buffer);
       	    GPSUpdate update(wxEVT_GPS_UPDATE, fix);
	        wxPostEvent(m_dest, update);
        }
    }
    else
        wxLogDebug(_T("GPS::RxData: NMEA checksum failed for input"));
}

GPRMC::GPRMC(char *sentence)
{
    wxASSERT(sentence);

    struct tm stamp;
    char digits[2];

    char *p = Field(sentence, 1);
    wxASSERT(p);
    strncpy(digits, p, 2);
    stamp.tm_hour = atoi(digits);
    strncpy(digits, p+2, 2);
    stamp.tm_min = atoi(digits);
    strncpy(digits, p+4, 2);
    stamp.tm_sec = atoi(digits);

    p = Field(sentence, 9);
    wxASSERT(p);
    strncpy(digits, p, 2);
    stamp.tm_mday = atoi(digits);
    strncpy(digits, p+2, 2);
    stamp.tm_mon = atoi(digits)-1;
    strncpy(digits, p+4, 2);
    stamp.tm_year = atoi(digits)+100;

    m_stamp = mktime(&stamp);
    m_valid = !strcmp(Field(sentence, 2), "A");
    m_fix.SetLatitude(Coord(sentence, 3));
    m_fix.SetLongitude(Coord(sentence, 5));
    sscanf(Field(sentence, 7), "%f", &m_speed);
    sscanf(Field(sentence, 8), "%f", &m_heading);
    sscanf(Field(sentence, 10), "%f", &m_magnetic);
    if (!strcmp(Field(sentence, 11), "W"))
        m_magnetic = -m_magnetic;
    m_mode = *Field(sentence, 12);
}

void GPRMC::AsString(char *buf)
{
    sprintf(buf, "%s %lf %lf %f %f %f, %s",
            ctime(&m_stamp),
            m_fix.Latitude(),
            m_fix.Longitude(),
            m_speed, m_heading, m_magnetic,
            m_valid ? "valid" : "invalid");
}
