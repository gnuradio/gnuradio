/* -*- c++ -*- */
/*
 * Copyright 2002 Free Software Foundation, Inc.
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

#ifndef _ATSC_RRC_BANDPASS_H_
#define _ATSC_RRC_BANDPASS_H_

#include <gnuradio/atsc/api.h>
#include <gnuradio/atsc/root_raised_cosine_impl.h>

class ATSC_API atsc_root_raised_cosine_bandpass : public atsc_root_raised_cosine
{
public:
  atsc_root_raised_cosine_bandpass (double center_freq) : _center_freq (center_freq) {}
  virtual std::vector<float> taps (double sampling_freq);

protected:
  double	_center_freq;
};


#endif /* _ATSC_RRC_BANDPASS_H_ */
