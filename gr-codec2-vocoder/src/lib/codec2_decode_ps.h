/* -*- c++ -*- */
/*
 * Copyright 2005 Free Software Foundation, Inc.
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
#ifndef INCLUDED_CODEC2_DECODE_PS_H
#define INCLUDED_CODEC2_DECODE_PS_H

#include <gr_sync_interpolator.h>

class codec2_decode_ps;
typedef boost::shared_ptr<codec2_decode_ps> codec2_decode_ps_sptr;

codec2_decode_ps_sptr codec2_make_decode_ps ();

/*!
 * \brief CODEC2 Vocoder Decoder
 * \ingroup vocoder_blk
 */
class codec2_decode_ps : public gr_sync_interpolator {
  void *d_codec2;

  friend codec2_decode_ps_sptr codec2_make_decode_ps ();
  codec2_decode_ps ();

public:
  ~codec2_decode_ps ();

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif /* INCLUDED_CODEC2_DECODE_PS_H */
