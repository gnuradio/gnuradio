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

#ifndef INCLUDED_DIGITAL_OFDM_SERIALIZER_VCC_IMPL_H
#define INCLUDED_DIGITAL_OFDM_SERIALIZER_VCC_IMPL_H

#include <gnuradio/digital/ofdm_serializer_vcc.h>

namespace gr {
  namespace digital {

    class ofdm_serializer_vcc_impl : public ofdm_serializer_vcc
    {
     private:
      int d_fft_len; //!< FFT length
      std::vector<std::vector<int> > d_occupied_carriers; //!< Which carriers/symbols carry data
      pmt::pmt_t d_packet_len_tag_key; //!< Key of the length tag
      pmt::pmt_t d_out_len_tag_key; //!< Key of the length tag
      const int d_symbols_skipped; //!< Start position in d_occupied_carriers
      pmt::pmt_t d_carr_offset_key; //!< Key of the carrier offset tag
      int d_curr_set; //!< Current position in d_occupied_carriers
      int d_symbols_per_set;

     protected:
      /*!
       * Calculate the number of scalar complex symbols given a number of
       * OFDM symbols.
       */
      int calculate_output_stream_length(const gr_vector_int &ninput_items);
      void update_length_tags(int n_produced, int n_ports);

     public:
      ofdm_serializer_vcc_impl(
	  int fft_len,
	  const std::vector<std::vector<int> > &occupied_carriers,
	  const std::string &len_tag_key,
	  const std::string &packet_len_tag_key,
	  int symbols_skipped,
	  const std::string &carr_offset_key,
	  bool input_is_shifted
      );
      ~ofdm_serializer_vcc_impl();

      int work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_OFDM_SERIALIZER_VCC_IMPL_H */

