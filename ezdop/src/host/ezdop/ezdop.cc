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

// Application specific includes
#include "ezdop.h"

// System includes (FIXME: autoconf these)
#include <cassert>
#include <cstdio>
#include <unistd.h>

ezdop::ezdop()
{
    d_device = new struct ftdi_context;
    if (ftdi_init(d_device))
	fprintf(stderr, "ftdi_init: %s", d_device->error_str);
    d_online = false;
    d_rotating = false;
    d_rate = EZDOP_DEFAULT_RATE;
    d_state = ST_HI;
    d_ant = 8; // FIXME: get from controller.h
    d_seq = 0;
    d_val = 0;
    d_in_phase = 0;
    d_quadrature = 0;
    d_rotating = false;
}

ezdop::~ezdop()
{
    assert(d_device);
    delete d_device;
}

bool ezdop::init()
{
    assert(d_device);

    d_online = false;

    // Attaches to first found device matching ID strings
    if (ftdi_usb_open(d_device, EZDOP_VENDORID, EZDOP_PRODUCTID)) {
        fprintf(stderr, "ftdi_usb_open: %s", d_device->error_str);
        return false;
    }

    d_online = true;
    reset();

    return d_online;
}

bool ezdop::finish()
{
    assert(d_device);

    if (!d_online)
        return true;

    if (d_rotating)
        stop_rotating();    

    if (ftdi_usb_close(d_device)) {
        fprintf(stderr, "ftdi_usb_close: %s", d_device->error_str);
        return false;
    }

    d_online = false;
    return true;
}

bool ezdop::reset()
{
    assert(d_device);
    assert(d_online);

    // Reset FTDI chipset
    if (ftdi_usb_reset(d_device)) {
        fprintf(stderr, "ftdi_usb_reset: %s", d_device->error_str);
        return false;
    }

    // Set FTDI chipset baudrate for bitbang
    if (ftdi_set_baudrate(d_device, EZDOP_BAUDRATE)) {
        fprintf(stderr, "ftdi_set_baudrate: %s", d_device->error_str);
        return false;
    }

    // Toggle DTR (-->AVR RESET)
    // Enable bitbang
    if (ftdi_enable_bitbang(d_device, EZDOP_BBDIR)) {
        fprintf(stderr, "ftdi_enable_bitbang: %s", d_device->error_str);
        return false;
    }

    // Lower DTR by writing 0 to bitbang output
    if (!send_byte(0x00)) // This actually lowers all outputs, not just DTR
        return false;

    // 10 ms sleep with RESET low
    usleep(10000);

    // Now raise DTR by writing 1 to bitbang output
    if (!send_byte(0xFF)) // This actually raises all outputs, not just DTR
        return false;

    if (ftdi_disable_bitbang(d_device)) {
        fprintf(stderr, "ftdi_disable_bitbang: %s", d_device->error_str);
        return false;
    }
	
    // Minimum chunk size for reads to reduce latency
    if (ftdi_read_data_set_chunksize(d_device, 256)) {
        fprintf(stderr, "ftdi_read_data_set_chunksize: %s", d_device->error_str);
        return false;
    }

    // 100 ms after RESET cleared to let things warm up
    usleep(100000);
    
    d_rate = EZDOP_DEFAULT_RATE;
    return true;
}

bool ezdop::set_rate(int rate)
{
    assert(d_device);
    assert(d_online);

    // Rate command is one byte, followed by rate as operand
    int divisor = 2000/rate;
    if (send_byte(EZDOP_CMD_RATE) && send_byte((unsigned char)divisor)) {
        d_rate = divisor;
        return true;
    }

    return false;
}

bool ezdop::rotate()
{
    assert(d_online);
    assert(d_device);

    d_rotating = send_byte(EZDOP_CMD_ROTATE);
    return d_rotating;
}

bool ezdop::stop_rotating()
{
    assert(d_online);
    assert(d_device);

    // TODO: set to antenna #1, perhaps do this in firmware instead
    d_rotating = send_byte(EZDOP_CMD_STOP);
    return d_rotating;
}

bool ezdop::stream()
{
    assert(d_online);
    assert(d_device);

    return (send_byte(EZDOP_CMD_STREAM));
}

bool ezdop::stop_streaming()
{
    assert(d_online);
    assert(d_device);

    return (send_byte(EZDOP_CMD_STROFF));
}

bool ezdop::send_byte(unsigned char data)
{
    assert(d_online);
    assert(d_device);

    if (ftdi_write_data(d_device, &data, 1) != 1) {
        fprintf(stderr, "ftdi_write_data: %s", d_device->error_str);
        return false;
    }
    
    return true;	
}

int ezdop::read_raw(unsigned char *buffer, unsigned int length)
{
    assert(d_online);
    assert(d_device);
    assert(buffer);
        
    // Read samples from USB port, 2 bytes per sample
    int rd = ftdi_read_data(d_device, buffer, length);
    if (rd < 0) {
        fprintf(stderr, "ftdi_read_data: %s", d_device->error_str);
        return -1;
    }

    return rd;
}

int ezdop::read_iq(complex<float> *buffer, unsigned int samples)
{
    assert(d_online);
    assert(d_device);
    assert(buffer);

    // 4 phases, d_rate samples per phase, 2 bytes per sample
    int bufsize = 8*d_rate*samples;
    unsigned char *raw = new unsigned char[bufsize];

    // Read until required bytes are read. Will block until bytes arrive.
    int rd = 0;
    while (rd < bufsize)
        rd += read_raw(&raw[rd], bufsize-rd);

    // Iterate through read bytes and invoke state machine
    int i = 0, j = 0;   // i index inputs, j indexes outputs
    unsigned char ant;

    while (i < bufsize) {
        unsigned char ch = raw[i++];
        if (d_state == ST_LO) {
            d_val = ch;                     // Save lo byte
            d_state = ST_HI;                // Switch states
            continue;                       // Done with this state
        }
        
        if (d_state == ST_HI) {
            unsigned char ant = ch >> 4;    // antenna is high nibble
            if (ant != d_ant) {             // Didn't get expected antenna
                // Abort current sequence
                d_ant = 8;                  
                d_seq = 0;
                d_val = 0;
                d_in_phase = 0; d_quadrature = 0;
                d_val = ch;                 // Act as if this were a lo byte instead
                continue;                   // Stay in ST_HI
            }
        }

        // Got correct antenna
        d_val |= (ch & 0x03) << 8;  // Mask off and save audio high value

        // This down-converts rotation frequency to exactly 0 Hz
        // while integrating audio response over duration of one antenna phase
        if (d_ant == 8)                 // +I
            d_in_phase += d_val;
        else if (d_ant == 4)            // +Q
            d_quadrature += d_val;
        else if (d_ant == 2)            // -I
            d_in_phase -= d_val;
        else if (d_ant == 1)            // -Q
            d_quadrature -= d_val;
        d_val = 0;
    
        // Update expected antenna and sequence
        if (++d_seq == d_rate)  {
            d_ant = d_ant >> 1;
            d_seq = 0;
            if (d_ant == 0) {           // fell off the end
                d_ant = 8;              // FIXME: grab from controller.h
    
                // We've accumulated I and Q over a whole antenna rotation
                // Output complex<float> in range [-1.0, 1.0]
                buffer[j++] = complex<float>(d_in_phase/(1024.0*d_rate), 
                                             d_quadrature/(1024.0*d_rate));
                d_in_phase = 0; d_quadrature = 0;
            }
        }

        d_state = ST_LO;  // Switch states
    };

    delete raw;
    return j;
}
