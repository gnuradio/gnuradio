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
#include "doppler.h"
#include "util.h"
#include <dopctrl.h>

// wxWidgets includes
#include <wx/log.h>
#include <wx/frame.h>

// Boost includes
#include <boost/scoped_array.hpp>

// System level includes
#include <cmath>

// TODO: read from ezdop.h
#define SAMPLERATE      8000
#define MAXSAMPLE       0x3FF      // 12 bit ADC
#define DEFAULT_SELECTED_ROTATION_RATE 2     // 500 Hz until told otherwise

#define QUANTUM         0.2        // Sample period in seconds
#define DEFAULT_FILTER_LEVEL 20

#define NORMALIZEPHASE(x) \
        if ((x) > M_PI) \
            (x) -= 2*M_PI; \
        if ((x) < -M_PI) \
            (x) += 2*M_PI;

unsigned char rotation_rates[] = {
    8,      // 250 Hz
    5,      // 400 Hz
    4,      // 500 Hz
    3,      // 666 Hz
    2,      // 1000 Hz
    1       // 2000 Hz
};

const wxEventType wxEVT_DOPPLER_UPDATE = wxNewEventType();

EZDopplerUpdate::EZDopplerUpdate(const wxEventType &event, float &in_phase, 
                                 float &quadrature, float &volume) :
wxNotifyEvent(event)
{
    m_in_phase = in_phase;
    m_quadrature = quadrature;
    m_volume = volume;
}

DopplerBackground::DopplerBackground(wxWindow *window, EZDoppler *doppler)
{
    wxASSERT(window);
    wxASSERT(doppler);
    
    m_running = false;
    m_dest = window;
    m_doppler = doppler;
	Create();
}

// It's in thread.h but somehow gets undef'd
typedef void *ExitCode;
ExitCode DopplerBackground::Entry()
{
    float in_phase, quadrature, phase, magnitude, volume, rflevel;

    m_running = true;
	while (!TestDestroy()) {
	    if (m_doppler->Sample(in_phase, quadrature, volume)) {
    	    EZDopplerUpdate update(wxEVT_DOPPLER_UPDATE, in_phase, quadrature, volume);
	        wxPostEvent(m_dest, update);
        }
	}
	m_running = false;
}

EZDoppler::EZDoppler(wxWindow *gui)
{
    wxASSERT(gui);

    m_thread = NULL;
    m_gui = gui;

    m_phase = complex<float>(0.0, 0.0);
    m_output = complex<float>(0.0, 0.0);
    m_alpha = complex<float>(0.0, 0.0);
    m_beta = complex<float>(0.0, 0.0);

    m_offset = 0.0;
    m_angle = 0.0;
        
    for(int i = 0; i < NUM_RATES; i++) 
        m_calibration[i] = 0.0;
    
    m_ezdop = ezdop_sptr(new ezdop());
    m_selected_rate = DEFAULT_SELECTED_ROTATION_RATE;
}

EZDoppler::~EZDoppler()
{
    if (m_ezdop->is_online()) {
        wxLogMessage(_T("EZDoppler::~EZDoppler(): doppler still online in destructor, finalizing"));
        Finalize();
    }
}

bool EZDoppler::Initialize()
{
    m_ezdop->init();
    if (m_ezdop->is_online())
        Reset();

    return m_ezdop->is_online();
}

bool EZDoppler::Finalize()
{
    if (m_thread && m_thread->IsRunning()) {
        wxLogDebug(_T("EZDoppler::Finalize: finalizing a running doppler"));
        Stop();
    }
}

bool EZDoppler::IsOnline()
{
    return m_ezdop->is_online();
}

bool EZDoppler::Reset()
{
    if (m_thread && m_thread->IsRunning()) {
        wxLogDebug(_T("EZDoppler::Reset: resetting running doppler"));
        Stop();
    }

    return m_ezdop->reset();
}

bool EZDoppler::Start()
{
    if (!(m_ezdop->rotate() && m_ezdop->stream()))
        return false;
        
    m_thread = new DopplerBackground(m_gui, this);
    m_thread->Run();    
}

bool EZDoppler::Stop()
{
    if (m_thread && m_thread->IsRunning()) {
        m_thread->Delete();
        while (m_thread->IsRunning()) {
            wxYield();
        }
    }
    
    m_thread = NULL;
    return (m_ezdop->stop_streaming() && m_ezdop->stop_rotating());
}

bool EZDoppler::SelectRotationRate(int n)
{
    wxASSERT(n >= 0 && n < 6);
    wxLogDebug(_T("EZDoppler::SelectRotationRate: %i %i"), n, (int)(2000/rotation_rates[n]));
    m_selected_rate = n;
    return m_ezdop->set_rate(2000/rotation_rates[n]);
}

int EZDoppler::GetRotationRate()
{
    return m_selected_rate;
}

bool EZDoppler::SetFilter(int n)
{
    float beta = 30.0/(n*m_ezdop->rate()); // Empirically determined

    m_alpha = complex<float>(1.0-beta, 0.0);
    m_beta = complex<float>(beta, 0.0);

    return true;
}

typedef boost::scoped_array<complex<float> > complexf_scoped_array;

// IQ is 2 complex floats, maximum rate is 2000, QUANTUM is period in seconds
complex<float> buffer[(int)(2*QUANTUM*2000)];

bool EZDoppler::Sample(float &in_phase, float &quadrature, float &volume)
{
    int nsamples = (int)(m_ezdop->rate()*QUANTUM);

    if (!m_ezdop->read_iq(buffer, nsamples, volume))
        return false;

    for (int i=0; i < nsamples; i++)
        m_phase = m_alpha*m_phase + m_beta*buffer[i];

    // m_angle is the actual instrument reading regardless of calibration
    m_angle = atan2(m_phase.imag(), m_phase.real());

    // Calibration angle is sum of equalized offset and global offset
    float cal_angle = m_calibration[m_selected_rate] + m_offset;

    // Rotate I, Q by calibration angle
    complex<float> cal = complex<float>(cos(cal_angle), sin(cal_angle));
    m_output = m_phase*cal;

    in_phase = m_output.real()*nsamples/512.0;
    quadrature = m_output.imag()*nsamples/512.0;
    // adjust volume
    
//  wxLogDebug(_T("%f %f %f"), in_phase, quadrature, volume);
    return true;
}

bool EZDoppler::Calibrate(float phase)
{

    float offset = phase - m_angle;
    NORMALIZEPHASE(offset);
    m_calibration[m_selected_rate] = offset;

    return true;
}

bool EZDoppler::SetCalibration(int rate, float offset)
{

    wxASSERT(rate >= 0 && rate < 7);
    if (rate < 6)
        m_calibration[rate] = offset;
    else
        m_offset = offset;

    return true;
}

float EZDoppler::GetCalibration(int rate)
{
    wxASSERT(rate >= 0 && rate < 7);
    if (rate < 6)
        return m_calibration[rate];
    else
        return m_offset;        

    return 0.0;
}

bool EZDoppler::SetOffset(float offset)
{
    m_offset = offset-m_angle-m_calibration[m_selected_rate];
    NORMALIZEPHASE(m_offset);
    NORMALIZEPHASE(m_offset);
    NORMALIZEPHASE(m_offset);
}

bool EZDoppler::Nudge(float amount)
{
    float cal = m_calibration[m_selected_rate];
    cal += amount;
    NORMALIZEPHASE(cal);
    m_calibration[m_selected_rate] = cal;

    return true;
}

bool EZDoppler::NudgeAll(float amount)
{
    m_offset += amount;
    NORMALIZEPHASE(m_offset);
    return true;
}
