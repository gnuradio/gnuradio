/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_OFDM_BPSK_MAPPER_H
#define INCLUDED_GR_OFDM_BPSK_MAPPER_H


#include <gr_block.h>
#include <gr_frame.h>
#include <vector>

class gr_ofdm_bpsk_mapper;
typedef boost::shared_ptr<gr_ofdm_bpsk_mapper> gr_ofdm_bpsk_mapper_sptr;

gr_ofdm_bpsk_mapper_sptr 
gr_make_ofdm_bpsk_mapper (unsigned mtu, unsigned occupied_carriers, unsigned int vlen,
			  std::vector<gr_complex> known_symbol1, 
			  std::vector<gr_complex> known_symbol2);

/*!
 * \brief take a stream of bytes in and map to a vector of complex
 * constellation points suitable for IFFT input to be used in an ofdm
 * modulator.  Simple BPSK version.
 */

class gr_ofdm_bpsk_mapper : public gr_block
{
  friend gr_ofdm_bpsk_mapper_sptr
    gr_make_ofdm_bpsk_mapper (unsigned mtu, unsigned occupied_carriers, unsigned int vlen,
			      std::vector<gr_complex> known_symbol1, 
			      std::vector<gr_complex> known_symbol2);
  
 protected:
  gr_ofdm_bpsk_mapper (unsigned mtu, unsigned occupied_carriers, unsigned int vlen,
		       std::vector<gr_complex> known_symbol1, 
		       std::vector<gr_complex> known_symbol2);
  
 private:
  unsigned int d_mtu;
  unsigned int d_occupied_carriers;
  unsigned int d_vlen;
  unsigned int d_packet_offset;
  unsigned int d_bit_offset;
  unsigned int d_header_sent;
  std::vector<gr_complex> d_known_symbol1, d_known_symbol2;

  void forecast (int noutput_items, gr_vector_int &ninput_items_required);

 public:
  ~gr_ofdm_bpsk_mapper(void);
  int general_work(int noutput_items,
		   gr_vector_int &ninput_items,
		   gr_vector_const_void_star &input_items,
		   gr_vector_void_star &output_items);

};


#endif
