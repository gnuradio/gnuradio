/* -*- c++ -*- */
/*
 * Copyright 2003,2004 Free Software Foundation, Inc.
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

#ifndef INCLUDED_SDR_1000_H
#define INCLUDED_SDR_1000_H

#include <gr_core_api.h>
#include <boost/shared_ptr.hpp>

class ppio;
typedef boost::shared_ptr<ppio> ppio_sptr;


enum { L_EXT = 0, L_BAND = 1, L_DDS0 = 2, L_DDS1 = 3 };

/*!
 * \brief Very low level interface to SDR 1000 xcvr hardware
 * \sa sdr_1000.py for a higher level interface.
 * \ingroup hardware
 */
class GR_CORE_API sdr_1000_base {
  ppio_sptr	d_ppio;
  int	 	d_shadow[4];	// shadow latches
  
public:

  sdr_1000_base (int which_pp);
  ~sdr_1000_base ();
  
  void reset ();
  void write_latch (int which, int value, int mask);
};

#endif /* INCLUDED_SDR_1000_H */
