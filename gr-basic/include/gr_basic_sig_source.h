/*
 * Copyright 2011 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */
#ifndef INCLUDED_GR_BASIC_SIG_SOURCE_H
#define INCLUDED_GR_BASIC_SIG_SOURCE_H

#include <gr_basic_api.h>
#include <gr_sync_block.h>
#include <gr_basic_op_types.h>
#include <complex>

class GR_BASIC_API gr_basic_sig_source : virtual public gr_sync_block{
public:
    typedef boost::shared_ptr<gr_basic_sig_source> sptr;

    //! Make a new signal source given data type
    static sptr make(op_type type);

    //! Set the waveform type (CONST, COSINE, RAMP, SQUARE)
    virtual void set_waveform(const std::string &) = 0;

    //! Set the offset, this is a free addition operation
    virtual void set_offset(const std::complex<double> &) = 0;

    //! Set the scaler, this is a free multiply scalar operation
    virtual void set_scaler(const std::complex<double> &) = 0;

    //! Set the frequency, this is a fractional number between -1 and 1
    virtual void set_frequency(const double) = 0;

    //! Convenience call to set frequency with sample rate
    void set_frequency(const double samp_rate, const double wave_freq){
        return this->set_frequency(wave_freq/samp_rate);
    }

};

#endif /* INCLUDED_GR_BASIC_SIG_SOURCE_H */
