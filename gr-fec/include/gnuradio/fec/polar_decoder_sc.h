/* -*- c++ -*- */
/* 
 * Copyright 2015 Free Software Foundation, Inc.
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


#ifndef INCLUDED_FEC_POLAR_DECODER_SC_H
#define INCLUDED_FEC_POLAR_DECODER_SC_H

#include <gnuradio/fec/api.h>
#include <gnuradio/fec/polar_decoder_common.h>


namespace gr {
  namespace fec {

    /*!
     * \brief Standard successive cancellation decoder for POLAR codes
     * It expects float input with bits mapped 1 --> 1, 0 --> -1
     * Or: f = 2.0 * bit - 1.0
     *
     */
    class FEC_API polar_decoder_sc : public polar_decoder_common
    {
    public:
      static generic_decoder::sptr make(int block_size, int num_info_bits, std::vector<int> frozen_bit_positions, std::vector<char> frozen_bit_values, bool is_packed = false);
      ~polar_decoder_sc();

      // FECAPI
      void generic_work(void *in_buffer, void *out_buffer);

    private:
      polar_decoder_sc(int block_size, int num_info_bits, std::vector<int> frozen_bit_positions, std::vector<char> frozen_bit_values, bool is_packed);

      unsigned int d_frozen_bit_counter;

      float* d_llr_vec;
      unsigned char* d_u_hat_vec;

      unsigned char retrieve_bit_from_llr(float llr, const int pos);
      void sc_decode(float* llrs, unsigned char* u);
    };

  } // namespace fec
} // namespace gr

#endif /* INCLUDED_FEC_POLAR_DECODER_SC_H */

