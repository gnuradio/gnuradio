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
#ifndef INCLUDED_ATSC_PAD_H
#define INCLUDED_ATSC_PAD_H

#include <atsc_api.h>
#include <gr_sync_decimator.h>

class atsc_pad;
typedef boost::shared_ptr<atsc_pad> atsc_pad_sptr;

ATSC_API atsc_pad_sptr atsc_make_pad();

/*!
 * \brief pad mpeg ts packets from 188 byte char to 
 * to 256 byte atsc_mpeg_packet
 * \ingroup atsc
 *
 * input: unsigned char; output: atsc_mpeg_packet
 */
class ATSC_API atsc_pad : public gr_sync_decimator
{
  friend ATSC_API atsc_pad_sptr atsc_make_pad();

  atsc_pad();

public:
  void forecast (int noutput_items, gr_vector_int &ninput_items_required);
  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);

  void reset() { /* nop */ }
};


#endif /* INCLUDED_ATSC_PAD_H */
