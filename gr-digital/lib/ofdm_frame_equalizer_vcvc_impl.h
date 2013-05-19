/* -*- c++ -*- */
/* Copyright 2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_OFDM_FRAME_EQUALIZER_VCVC_IMPL_H
#define INCLUDED_DIGITAL_OFDM_FRAME_EQUALIZER_VCVC_IMPL_H

#include <gnuradio/digital/ofdm_frame_equalizer_vcvc.h>

namespace gr {
  namespace digital {

    class ofdm_frame_equalizer_vcvc_impl : public ofdm_frame_equalizer_vcvc
    {
     private:
      const int d_fft_len;
      const int d_cp_len;
      ofdm_equalizer_base::sptr d_eq;
      bool d_propagate_channel_state;
      const int d_fixed_frame_len;
      std::vector<gr_complex> d_channel_state;

     protected:
      void parse_length_tags(
	  const std::vector<std::vector<tag_t> > &tags,
	  gr_vector_int &n_input_items_reqd
      );

     public:
      ofdm_frame_equalizer_vcvc_impl(
	  ofdm_equalizer_base::sptr equalizer,
	  int cp_len,
	  const std::string &len_tag_key,
	  bool propagate_channel_state,
	  int fixed_frame_len
      );
      ~ofdm_frame_equalizer_vcvc_impl();

      int work(int noutput_items,
	  gr_vector_int &ninput_items,
	  gr_vector_const_void_star &input_items,
	  gr_vector_void_star &output_items);
    };

  } // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_OFDM_FRAME_EQUALIZER_VCVC_IMPL_H */

