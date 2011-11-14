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

#ifndef INCLUDED_GR_FILTER_DECIM_FIR_H
#define INCLUDED_GR_FILTER_DECIM_FIR_H

#include <gr_filter_api.h>
#include <gr_sync_decimator.h>
#include <gr_filter_types.h>
#include <complex>

class GR_FILTER_API gr_filter_decim_fir : virtual public gr_sync_decimator{
public:
    typedef boost::shared_ptr<gr_filter_decim_fir> sptr;

    //! Make a new FIR filter given data type
    static sptr make(filter_type type, const size_t decim = 1);

    //! Set the taps for this FIR filter
    virtual void set_taps(const std::vector<std::complex<double> > &taps) = 0;

};

#endif /* INCLUDED_GR_FILTER_DECIM_FIR_H */
