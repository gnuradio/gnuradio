/* -*- c++ -*- */
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

#ifndef INCLUDED_DIGITAL_CONSTELLATION_DECODER_CB_H
#define INCLUDED_DIGITAL_CONSTELLATION_DECODER_CB_H

#include <digital_api.h>
#include <gr_block.h>
#include <digital_constellation.h>
#include <vector>

class digital_constellation_decoder_cb;
typedef boost::shared_ptr<digital_constellation_decoder_cb>digital_constellation_decoder_cb_sptr;

DIGITAL_API digital_constellation_decoder_cb_sptr 
digital_make_constellation_decoder_cb (digital_constellation_sptr constellation);

/*!
 * \brief Constellation Decoder
 * \ingroup coding_blk
 * \ingroup digital
 *
 */
class DIGITAL_API digital_constellation_decoder_cb : public gr_block
{

 private:
  digital_constellation_sptr d_constellation;
  unsigned int d_dim;

  friend DIGITAL_API digital_constellation_decoder_cb_sptr 
  digital_make_constellation_decoder_cb (digital_constellation_sptr constellation);
  
  digital_constellation_decoder_cb (digital_constellation_sptr constellation);

 public:

  void forecast (int noutput_items,
		 gr_vector_int &ninput_items_required);
  
  int general_work (int noutput_items,
		    gr_vector_int &ninput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items);
};

#endif
