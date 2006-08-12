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
#include "doppler.h"
#include "util.h"
#include <dopctrl.h>

// wxWidgets includes
#include <wx/log.h>
#include <wx/frame.h>

// System level includes
#include <cmath>

#define SAMPLERATE      8000
#define QUANTUM         0.2        // Sample period in seconds
#define MAXSAMPLE       0x3FF      // 12 bit ADC

#define DEFAULT_SELECTED_ROTATION_RATE 2     // 500 Hz until told otherwise
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
	    if (m_doppler->Sample((int)(QUANTUM*SAMPLERATE), in_phase, quadrature, volume)) {
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
    m_online = false;
    m_selected_rate = DEFAULT_SELECTED_ROTATION_RATE;
    m_gui = gui;
    m_in_phase = 0.0;
    m_quadrature = 0.0;
    m_alpha = 1.0/(DEFAULT_FILTER_LEVEL*200);
    m_beta = 1.0-m_alpha;
    m_phase = 0.0;
    m_offset = 0.0;
        
    for(int i = 0; i < NUM_RATES; i++) 
        m_calibration[i] = 0.0;
    
#if HAVE_LIBFTDI
	m_device = new struct ftdi_context;
	wxASSERT(m_device);
	if (ftdi_init(m_device)) {
	    wxLogWarning(_T("ftdi_init: %s"), m_device->error_str);
	    return;
    }
#endif

}

EZDoppler::~EZDoppler()
{
    if (m_online) {
        wxLogMessage(_T("EZDoppler::~EZDoppler(): doppler still online in destructor, finalizing"));
        Finalize();
    }
#if HAVE_LIBFTDI
    wxASSERT(m_device);
	ftdi_deinit(m_device);
	delete m_device;
#endif
}

bool EZDoppler::Initialize()
{
    m_online = false;
    
#if HAVE_LIBFTDI
	if (ftdi_usb_open(m_device, EZDOP_VENDORID, EZDOP_PRODUCTID)) {
		wxLogDebug(_T("ftdi_usb_open: %s"), m_device->error_str);
		return false;
	}
#elif HAVE_LIBFTD2XX
    if ((m_status = FT_Open(0, &m_handle)) != FT_OK) {
		wxLogError(_T("FT_Open failed: %i"), m_status);
		return false;
	}
#endif

    m_online = true;
    if (m_online)
        Reset();

    return m_online;
 }

bool EZDoppler::Finalize()
{
    if (!m_online)
        return true;

    if (m_thread && m_thread->IsRunning()) {
        wxLogDebug(_T("EZDoppler::Finalize: finalizing a running doppler"));
        Stop();
    }

#if HAVE_LIBFTDI
	if (ftdi_usb_close(m_device)) {
	    wxLogWarning(_T("ftdi_usb_close: %s"), m_device->error_str);
	    return false;
	}
#elif HAVE_LIBFTD2XX
    if ((m_status = FT_Close(m_handle)) != FT_OK) {
		wxLogWarning(_T("FT_Close failed: %i"), m_status);
		return false;
	}
#endif

    m_online = false;
    return true;
}

bool EZDoppler::IsOnline()
{
    return m_online;
}

bool EZDoppler::send_byte(unsigned char data)
{
    wxASSERT(m_online);
#if HAVE_LIBFTDI	
    if (ftdi_write_data(m_device, &data, 1) != 1) {
        wxLogWarning(_T("ftdi_write_data: %s"), m_device->error_str);
        return false;
    }
#elif HAVE_LIBFTD2XX
    DWORD written;
    if ((m_status = FT_Write(m_handle, &data, 1, &written)) != FT_OK || written != 1) {
        wxLogError(_T("FT_Write failed: %i"), m_status);
		return false;
	}
#endif
	return true;	
}

bool EZDoppler::Reset()
{
    wxASSERT(m_online);

    if (m_thread && m_thread->IsRunning()) {
        wxLogDebug(_T("EZDoppler::Reset: resetting running doppler"));
        Stop();
    }


    // Reset FTDI chipset
#if HAVE_LIBFTDI
	if (ftdi_usb_reset(m_device)) {
	    wxLogWarning(_T("ftdi_usb_reset: %s"), m_device->error_str);
	    return false;
    }
#elif HAVE_LIBFTD2XX
	if ((m_status = FT_ResetDevice(m_handle) != FT_OK)) {
		wxLogError(_T("FT_ResetDevice failed: %i"), m_status);
		return false;
	}
#endif

    // Set FTDI chipset baudrate for bitbang
#if HAVE_LIBFTDI
	if (ftdi_set_baudrate(m_device, EZDOP_BAUDRATE)) {
	    wxLogWarning(_T("ftdi_set_baudrate: %s"), m_device->error_str);
	    return false;
    }
#elif HAVE_LIBFTD2XX
	if ((m_status = FT_SetBaudRate(m_handle, EZDOP_BAUDRATE)) != FT_OK) {
		wxLogError(_T("FT_SetBaudRate failed: %i"), m_status);
		return false;
	}
#endif

    // Toggle DTR (-->AVR RESET)
#if HAVE_LIBFTDI
    // Enable bitbang
	if (ftdi_enable_bitbang(m_device, EZDOP_BBDIR)) {
	    wxLogWarning(_T("ftdi_enable_bitbang: %s"), m_device->error_str);
		return false;
	}

	// Lower DTR by writing 0 to bitbang output
	if (!send_byte(0x00)) // HMMM: this actually lowers all outputs, of course
	    return false;
#elif HAVE_LIBFTD2XX
	// Set DTR line (goes low) to reset AVR and delay
	if ((m_status = FT_SetDtr(m_handle)) != FT_OK) {
		wxLogError(_T("FT_SetDtr failed: %i"), m_status);
		return false;
	}
#endif

    // 10 ms sleep with RESET low
    wxMilliSleep(10); 

#if HAVE_LIBFTDI
	// Now raise DTR by writing 1 to bitbang output
	if (!send_byte(0xFF))
	    return false;

	if (ftdi_disable_bitbang(m_device)) {
	    wxLogWarning(_T("ftdi_disable_bitbang: %s"), m_device->error_str);
		return false;
	}
	
	// Minimum chunk size for reads to reduce latency
	if (ftdi_read_data_set_chunksize(m_device, 256)) {
	    wxLogWarning(_T("ftdi_read_data_set_chunksize: %s"), m_device->error_str);
		return false;
	}
#elif HAVE_LIBFTD2XX
    if ((m_status = FT_ClrDtr(m_handle)) != FT_OK) {
		wxLogError(_T("FT_ClrDtr failed: %i"), m_status);
		return false;
	}
#endif

    // 100 ms after RESET cleared to let things warm up
    wxMilliSleep(100);

    m_selected_rate = DEFAULT_SELECTED_ROTATION_RATE;

    return true;
}

bool EZDoppler::Start()
{
    wxASSERT(m_online);
    // TODO: flush stream data

    if (!send_byte(EZDOP_CMD_ROTATE) || !send_byte(EZDOP_CMD_STREAM))
        return false;
        
    m_thread = new DopplerBackground(m_gui, this);
    m_thread->Run();    
}

bool EZDoppler::Stop()
{
    wxASSERT(m_online);
    // TODO: flush stream data

    if (m_thread && m_thread->IsRunning()) {
        m_thread->Delete();
        while (m_thread->IsRunning()) {
            wxYield();
        }
    }
    
    m_thread = NULL;
    return (send_byte(EZDOP_CMD_STROFF) && send_byte(EZDOP_CMD_STOP));
}

bool EZDoppler::SelectRotationRate(int n)
{
    wxASSERT(m_online);
    wxASSERT(n >= 0 && n < 6);

    unsigned char rate = rotation_rates[n];
    if (send_byte(EZDOP_CMD_RATE) && send_byte(rate)) {
        m_selected_rate = n;
        m_in_phase = 0.0;
        m_quadrature = 0.0;
        return true;
    }
    
    return false;
}

int EZDoppler::GetSelectedRotationRate()
{
    return m_selected_rate;
}

bool EZDoppler::Zero()
{
    return true;
}

bool EZDoppler::SetFilter(int n)
{
    wxASSERT(n > 0);
    m_alpha = 1.0/(n*200); // Time constant is filter value divided by 5 (empirically determined)
    m_beta = 1.0-m_alpha;
    return true;
}

bool EZDoppler::Sample(int nsamples, float &in_phase, float &quadrature, float &volume)
{
    unsigned short *audio = new unsigned short[nsamples*2];
    unsigned char *antenna = new unsigned char[nsamples];
    
    unsigned int rd;
    unsigned int count = 0;
    
    // Read samples from USB port, 2 bytes per sample
    while (count < nsamples*2) {
        unsigned int amt = nsamples*2-count;
        unsigned char *ptr = (unsigned char *)&audio[count/2]; // if count is odd, causes frame slip?
        if ((count/2)*2 != count)
            wxLogDebug(_T("EZDoppler::Sample: count is odd (%i)"), count);
#if HAVE_LIBFTDI
        rd = ftdi_read_data(m_device, ptr, amt);
        if (rd < 0) {
            wxLogWarning(_T("ftdi_read_data: %s"), m_device->error_str);
            return false; // FIXME: memory leak for antenna and audio!
        }
        count += rd;
#elif HAVE_LIBFTD2XX
        DWORD num;
        FT_STATUS status = FT_Read(m_handle, ptr, amt, &num);
        if (status != FT_OK) {
            wxLogWarning(_T("FT_Read: %i"), status);
            return false; // FIXME: memory leak for antenna and audio!
        }
        count += num;
#endif        
    }    
    
    // Extract antenna array position from samples, flag unsynced if not a valid antenna value
    bool sync = true;
    for (int i = 0; i < nsamples; i++) {
        unsigned char ant = (audio[i] & 0xF000) >> 12;
        if (ant != 8 && ant != 4 && ant != 2 && ant != 1)
            sync = false;
        antenna[i] = ant;
        audio[i] &= 0x03FF;
    }
            
    // If not synced, throw away a byte in receive stream to resync
    unsigned char dummy;
    if (!sync) {
        wxLogDebug(_T("EZDoppler::Sample: sync failure detected"));
#if HAVE_LIBFTDI
        ftdi_read_data(m_device, &dummy, 1);
#elif HAVE_LIBFTD2XX
        DWORD rd;
        FT_Read(m_handle, &dummy, 1, &rd);
#endif            
        return false; // FIXME: memory leak for antenna and audio!
    }

    // Calculate DC offset and max and min values
    float sum = 0.0;
    float mean = 0.0;
    for (int i = 0; i < nsamples; i++)
        sum += audio[i];
    mean = sum/nsamples;

    // Calculate doppler response
    unsigned char ant;
    float sample;
    volume = 0.0;
    for (int i = 0; i < nsamples; i++) {
        ant = antenna[i];

        // Subtract DC offset and scale to -1 to 1
        sample = 2*(((float)audio[i])-mean)/MAXSAMPLE;

        // Calculate peak volume
        if (fabs(sample) > volume)
            volume = fabs(sample);

        // Integrate and lowpass filter sample into I/Q based on which antenna is selected
        // Order here creates a clockwise rotating I/Q phasor
        switch(ant) {
            case 8:
                m_in_phase = m_in_phase*m_beta + sample*m_alpha;
                break;
            case 4:
                m_quadrature = m_quadrature*m_beta - sample*m_alpha;
                break;
            case 2:
                m_in_phase = m_in_phase*m_beta - sample*m_alpha;
                break;
            case 1:
                m_quadrature = m_quadrature*m_beta + sample*m_alpha;
                break;
            default:
                wxLogError(_T("EZDoppler::Sample: Unknown antenna value %i"), ant);
                break;
        }
    }

    // m_phase is the actual instrument reading regardless of calibration
    m_phase = atan2(m_quadrature, m_in_phase);

    // Calibration angle is sum of equalized offset and global offset
    float cal = m_calibration[m_selected_rate] + m_offset;

    // Rotate I, Q by calibration angle
    float i_cal = cos(cal);
    float q_cal = sin(cal);
    in_phase = m_in_phase*i_cal - m_quadrature*q_cal;
    quadrature = m_quadrature*i_cal + m_in_phase*q_cal;

    delete antenna;
    delete audio;
    return true;
}

bool EZDoppler::Calibrate(float phase)
{
    float offset = phase - m_phase;
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
}

float EZDoppler::GetCalibration(int rate)
{
    wxASSERT(rate >= 0 && rate < 7);
    if (rate < 6)
        return m_calibration[rate];
    else
        return m_offset;        
}

bool EZDoppler::SetOffset(float offset)
{
    m_offset = offset-m_phase-m_calibration[m_selected_rate];
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
