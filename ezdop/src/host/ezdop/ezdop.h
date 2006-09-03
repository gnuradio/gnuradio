/*
 * Copyright 2006 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __EZDOP_H__
#define __EZDOP_H__

// Application level includes
#include <dopctrl.h>

// System level includes
#include <ftdi.h>
#include <complex>

using std::complex;

class ezdop
{
public:
    ezdop();
    ~ezdop();

    // Housekeeping
    bool init();		 // Attach to first device found, TODO: serial no
    bool finish();		 // Release device
    bool reset();		 // Reset state to post-init()

    // Parameters
    bool set_rate(int rate); 	 // Set rotation rate
    int rate() const	 	 // Get rotation rate
        { return 2000/d_rate; }

    // Commands
    bool rotate();               // Start antenna rotation
    bool stop_rotating();	 // Stop antenna rotation
    bool stream();               // Start audio streaming
    bool stop_streaming();       // Stop audio streaming
        
    // Raw read of sample stream from device, length in bytes (2 per sample)
    int read_raw(unsigned char *buffer, unsigned int length);

    // Read synced, downconverted I and Q samples, one per rotation
    int read_iq(complex<float> *buffer, unsigned int samples, float &volume);

    // Status
    bool is_online() const { return d_online; }

private:
    struct ftdi_context  *d_device;     // libftdi device instance data
    bool d_online;     // Successfully found and connected to device
    bool d_rotating;   // Doppler is rotating
    int  d_rate;       // Current rotation rate (samples per antenna)
    enum { ST_HI, ST_LO } d_state;      // Current byte sync state
    char d_ant;        // Current antenna being sequenced
    int  d_seq;        // Current phase sample number
    int  d_val;        // Current reassembled audio sample value
    int  d_in_phase;   // Downconverted I accumulator
    int  d_quadrature; // Downconverted Q accumulator
    
    bool send_byte(unsigned char data); // Send a byte to AVR

};

#endif
