/* -*- c++ -*- */
/*
 * Copyright 2006,2007,2011,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_OFDM_MAPPER_BCV_IMPL_H
#define INCLUDED_DIGITAL_OFDM_MAPPER_BCV_IMPL_H

#include <gnuradio/digital/ofdm_mapper_bcv.h>
#include <gnuradio/message.h>
#include <vector>

namespace gr {
  namespace digital {

    class ofdm_mapper_bcv_impl : public ofdm_mapper_bcv
    {
    private:
      std::vector<gr_complex> d_constellation;
      msg_queue::sptr d_msgq;
      message::sptr   d_msg;
      unsigned        d_msg_offset;
      bool            d_eof;
  
      unsigned int d_occupied_carriers;
      unsigned int d_fft_length;
      unsigned int d_bit_offset;
      int          d_pending_flag;

      unsigned long d_nbits;
      unsigned char d_msgbytes;
  
      unsigned char d_resid;
      unsigned int  d_nresid;

      std::vector<int> d_subcarrier_map;

      int randsym();

    public:
      ofdm_mapper_bcv_impl(const std::vector<gr_complex> &constellation,
			   unsigned msgq_limit, 
			   unsigned occupied_carriers,
			   unsigned int fft_length);
      ~ofdm_mapper_bcv_impl(void);

      msg_queue::sptr msgq() const { return d_msgq; }

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_OFDM_MAPPER_BCV_IMPL_H */
