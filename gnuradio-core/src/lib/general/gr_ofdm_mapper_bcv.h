/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_OFDM_MAPPER_BCV_H
#define INCLUDED_GR_OFDM_MAPPER_BCV_H

#include <gr_sync_decimator.h>

class gr_ofdm_mapper_bcv;
typedef boost::shared_ptr<gr_ofdm_mapper_bcv> gr_ofdm_mapper_bcv_sptr;

gr_ofdm_mapper_bcv_sptr 
gr_make_ofdm_mapper_bcv (unsigned int bits_per_symbol, unsigned int vlen);


/*!
 * \brief take a stream of bytes in and map to a vector of complex
 * constellation points suitable for IFFT input to be used in an ofdm
 * modulator.  Abstract class must be subclassed with specific mapping.
 *
 */

class gr_ofdm_mapper_bcv : public gr_sync_decimator
{
  friend gr_ofdm_mapper_bcv_sptr
  gr_make_ofdm_mapper_bcv (unsigned int bits_per_symbol, unsigned int vlen);

protected:
  gr_ofdm_mapper_bcv (unsigned int bits_per_symbol, unsigned int vlen);

public:
  ~gr_ofdm_mapper_bcv(void);
};



#endif
