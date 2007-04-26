/* -*- c++ -*- */
/*
 * Copyright 2004,2005 Free Software Foundation, Inc.
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
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_ofdm_mapper_bcv.h>
#include <gr_io_signature.h>

gr_ofdm_mapper_bcv::~gr_ofdm_mapper_bcv(void)
{
}

gr_ofdm_mapper_bcv::gr_ofdm_mapper_bcv (unsigned bits_per_symbol,unsigned int vlen)
  : gr_sync_decimator ("ofdm_mapper_bcv",
		       gr_make_io_signature (1, 1, sizeof(unsigned char)),
		       gr_make_io_signature (1, 1, sizeof(gr_complex)*vlen),
		       bits_per_symbol)
{
}

