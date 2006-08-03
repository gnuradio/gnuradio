/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <gr_io_signature.h>
#include <stdexcept>
#include <iostream>

gr_io_signature::gr_io_signature (int min_streams, int max_streams, size_t sizeof_stream_item)
{
  if (min_streams < 0
      || (max_streams != IO_INFINITE && max_streams < min_streams))
    throw std::invalid_argument ("gr_io_signature");

  d_min_streams = min_streams;
  d_max_streams = max_streams;
  d_sizeof_stream_item = sizeof_stream_item;
}

gr_io_signature::~gr_io_signature ()
{
  // NOP for now
  // std::cout << "destroying gr_io_signature: " << this << '\n';
}

gr_io_signature_sptr
gr_make_io_signature (int min_streams, int max_streams, size_t sizeof_stream_item)
{
  return gr_io_signature_sptr (new gr_io_signature (min_streams, max_streams,
						   sizeof_stream_item));
}


