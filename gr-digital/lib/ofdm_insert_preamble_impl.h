/* -*- c++ -*- */
/*
 * Copyright 2007,2011,2012 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef INCLUDED_DIGITAL_OFDM_INSERT_PREAMBLE_IMPL_H
#define INCLUDED_DIGITAL_OFDM_INSERT_PREAMBLE_IMPL_H

#include <gnuradio/digital/ofdm_insert_preamble.h>

namespace gr {
  namespace digital {
    
    class ofdm_insert_preamble_impl : public ofdm_insert_preamble
    {
    private:
      enum state_t {
	ST_IDLE,
	ST_PREAMBLE,
	ST_FIRST_PAYLOAD,
	ST_PAYLOAD
      };

      int     d_fft_length;
      state_t d_state;
      int     d_nsymbols_output;
      int     d_pending_flag;
      const std::vector<std::vector<gr_complex> > d_preamble;

      void enter_idle();
      void enter_first_payload();
      void enter_payload();
  
    public:
      ofdm_insert_preamble_impl(int fft_length,
				const std::vector<std::vector<gr_complex> > &preamble);
      ~ofdm_insert_preamble_impl();

      void enter_preamble();

      int general_work(int noutput_items,
		    gr_vector_int &ninput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items);
      void forecast(int noutput_items, gr_vector_int &ninput_items_required);
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_OFDM_INSERT_PREAMBLE_IMPL_H */
