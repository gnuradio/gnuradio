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

#ifndef INCLUDED_GR_OFDM_BPSK_DEMAPPER_H
#define INCLUDED_GR_OFDM_BPSK_DEMAPPER_H


#include <gr_core_api.h>
#include <gr_block.h>
#include <vector>

class gr_ofdm_bpsk_demapper;
typedef boost::shared_ptr<gr_ofdm_bpsk_demapper> gr_ofdm_bpsk_demapper_sptr;

GR_CORE_API gr_ofdm_bpsk_demapper_sptr
gr_make_ofdm_bpsk_demapper (unsigned int occupied_carriers);


/*!
 * \brief take a vector of complex constellation points in from an FFT
 * and demodulate to a stream of bits. Simple BPSK version.
 * \ingroup ofdm_blk
 */
class GR_CORE_API gr_ofdm_bpsk_demapper : public gr_block
{
  friend GR_CORE_API gr_ofdm_bpsk_demapper_sptr
    gr_make_ofdm_bpsk_demapper (unsigned int occupied_carriers);

 protected:
  gr_ofdm_bpsk_demapper (unsigned int occupied_carriers);

 private:
  unsigned char slicer(gr_complex x);

  unsigned int d_occupied_carriers;
  unsigned int d_byte_offset;
  unsigned char d_partial_byte;

  void forecast(int noutput_items, gr_vector_int &ninput_items_required);

 public:
  ~gr_ofdm_bpsk_demapper(void);
  int general_work(int noutput_items,
		   gr_vector_int &ninput_items,
		   gr_vector_const_void_star &input_items,
		   gr_vector_void_star &output_items);
};


#endif
