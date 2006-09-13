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

#ifndef __DOPPLER_H__
#define __DOPPLER_H__

// Autoconf generated configure options
#if HAVE_CONFIG_H
    #include "config.h"
#endif

// Application level includes
#include <ezdop.h>
#include <boost/shared_ptr.hpp>
#include <wx/event.h>

// TODO: Read this from ezdop.h
#define NUM_RATES   6   

class EZDoppler;

class DopplerBackground : public wxThread
{
public:
        DopplerBackground(wxWindow *window, EZDoppler *doppler);
        virtual ExitCode Entry();
        bool IsRunning() { return m_running; }

private:
        bool m_running;
        wxWindow *m_dest;
        EZDoppler *m_doppler;
};

class EZDopplerUpdate : public wxNotifyEvent
{
public:
    EZDopplerUpdate(const wxEventType &event, float &in_phase, float &quadrature,
                    float &volume); 
    virtual wxEvent *Clone() const { return new EZDopplerUpdate(*this); }

    float m_in_phase;
    float m_quadrature;
    float m_volume;
};

extern const wxEventType wxEVT_DOPPLER_UPDATE;

typedef void(wxEvtHandler::*EZDopplerUpdateFunction)(EZDopplerUpdate&);

#define EVT_DOPPLER_UPDATE(fn) \
        DECLARE_EVENT_TABLE_ENTRY( \
            wxEVT_DOPPLER_UPDATE, -1, -1, \
            (wxObjectEventFunction)(wxEventFunction)(EZDopplerUpdateFunction)&fn, \
            (wxObject *)NULL \
        ),

typedef boost::shared_ptr<ezdop> ezdop_sptr;

class EZDoppler
{
public:
    EZDoppler(wxWindow *gui);
    ~EZDoppler();

    // Control commands
    bool Initialize();
    bool Finalize();
    bool IsOnline();
    bool Start();
    bool Stop();
    bool SetFilter(int n);
    bool SelectRotationRate(int n);
    int  GetRotationRate();
    bool Reset();
    bool Sample(float &in_phase, float &quadrature, float &volume);
    bool Calibrate(float phase);
    bool SetCalibration(int rate, float offset);
    float GetCalibration(int rate);
    bool SetOffset(float offset = 0.0);
    bool Nudge(float amount);
    bool NudgeAll(float amount);
            
private:
    ezdop_sptr m_ezdop;
    int m_selected_rate;
    wxWindow *m_gui;
    DopplerBackground *m_thread;

    complex<float> m_phase;         // Actual phase of doppler before calibration
    complex<float> m_output;        // Calibrated output phase
    complex<float> m_alpha;         // Exponential average constant
    complex<float> m_beta;          // Exponential average constant
    
    float m_angle;                  // Actual angle of doppler before calibration
    float m_offset;                 // Global calibration angle
    float m_calibration[NUM_RATES]; // Individual rotation rate offset
};    

#endif // __DOPPLER_H__
