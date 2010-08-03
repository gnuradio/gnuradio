/* -*- c++ -*- */
/*
 * Copyright 2004,2006,2007,2010 Free Software Foundation, Inc.
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

#include <gr_file_sink.h>
#include <gr_io_signature.h>
#include <stdexcept>


gr_file_sink_sptr
gr_make_file_sink (size_t itemsize, const char *filename)
{
  return gnuradio::get_initial_sptr(new gr_file_sink (itemsize, filename));
}

gr_file_sink::gr_file_sink(size_t itemsize, const char *filename)
  : gr_sync_block ("file_sink",
		   gr_make_io_signature(1, 1, itemsize),
		   gr_make_io_signature(0, 0, 0)),
    gr_file_sink_base(filename, true),
    d_itemsize(itemsize)
{
  if (!open(filename))
    throw std::runtime_error ("can't open file");
}

gr_file_sink::~gr_file_sink ()
{
}

int 
gr_file_sink::work (int noutput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items)
{
  char *inbuf = (char *) input_items[0];
  int  nwritten = 0;

  do_update();				// update d_fp is reqd
  
  if (!d_fp)
    return noutput_items;		// drop output on the floor

  while (nwritten < noutput_items){
    int count = fwrite (inbuf, d_itemsize, noutput_items - nwritten, d_fp);
    if (count == 0)	// FIXME add error handling
      break;
    nwritten += count;
    inbuf += count * d_itemsize;
  }
  if (d_unbuffered)
	  fflush (d_fp);
	  
  return nwritten;
}
