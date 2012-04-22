/* -*- c++ -*- */
/*
 * Copyright 2004,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_SIMPLE_FRAMER_H
#define INCLUDED_GR_SIMPLE_FRAMER_H

#include <digital_api.h>
#include <gr_block.h>

class digital_simple_framer;
typedef boost::shared_ptr<digital_simple_framer> digital_simple_framer_sptr;

DIGITAL_API digital_simple_framer_sptr digital_make_simple_framer(int payload_bytesize);

/*!
 * \brief add sync field, seq number and command field to payload
 * \ingroup sync_blk
 *
 * Takes in enough samples to create a full output frame. The frame is
 * prepended with the GRSF_SYNC (defind in
 * digital_simple_framer_sync.h) and an 8-bit sequence number.
 *
 * \param payload_bytesize The size of the payload in bytes.
 */
class DIGITAL_API digital_simple_framer : public gr_block
{
  int	d_seqno;
  int	d_payload_bytesize;
  int	d_input_block_size;	// bytes
  int	d_output_block_size;	// bytes

  friend DIGITAL_API digital_simple_framer_sptr
    digital_make_simple_framer(int payload_bytesize);
  digital_simple_framer(int payload_bytesize);

 public:
  void forecast(int noutput_items,
		gr_vector_int &ninput_items_required);

  int general_work(int noutput_items,
		   gr_vector_int &ninput_items,
		   gr_vector_const_void_star &input_items,
		   gr_vector_void_star &output_items);
};


#endif /* INCLUDED_GR_SIMPLE_FRAMER_H */
