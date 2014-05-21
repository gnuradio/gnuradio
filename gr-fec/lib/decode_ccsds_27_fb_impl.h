/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_FEC_DECODE_CCSDS_27_FB_IMPL_H
#define INCLUDED_FEC_DECODE_CCSDS_27_FB_IMPL_H

#include <gnuradio/fec/decode_ccsds_27_fb.h>

extern "C" {
#include <gnuradio/fec/viterbi.h>
}

namespace gr {
  namespace fec {

    class FEC_API decode_ccsds_27_fb_impl : public decode_ccsds_27_fb
    {
    private:
      // Viterbi state
      int d_mettab[2][256];
      struct viterbi_state d_state0[64];
      struct viterbi_state d_state1[64];
      unsigned char d_viterbi_in[16];

      int d_count;

    public:
      decode_ccsds_27_fb_impl();

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_DECODE_CCSDS_27_FB_IMPL_H */
