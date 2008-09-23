/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <usrp2_sink_base.h>
#include <gr_io_signature.h>
#include <iostream>

#define USRP2_SINK_BASE_DEBUG 0

usrp2_sink_base::usrp2_sink_base(const char *name,
				 gr_io_signature_sptr input_signature,
				 const std::string &ifc,
				 const std::string &mac) 
  throw (std::runtime_error)
  : usrp2_base(name,
               input_signature,
	       gr_make_io_signature(0, 0, 0),
	       ifc, mac)
{
  // NOP
}

usrp2_sink_base::~usrp2_sink_base ()
{
  // NOP
}

bool
usrp2_sink_base::set_gain(double gain)
{
  return d_u2->set_tx_gain(gain);
}

bool
usrp2_sink_base::set_center_freq(double frequency, usrp2::tune_result *tr)
{
  return d_u2->set_tx_center_freq(frequency, tr);
}

bool
usrp2_sink_base::set_interp(int interp_factor)
{
  return d_u2->set_tx_interp(interp_factor);
}
