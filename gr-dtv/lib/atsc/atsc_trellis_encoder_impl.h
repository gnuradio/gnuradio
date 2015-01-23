/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_DTV_ATSC_TRELLIS_ENCODER_IMPL_H
#define INCLUDED_DTV_ATSC_TRELLIS_ENCODER_IMPL_H

#include <gnuradio/dtv/atsc_trellis_encoder.h>
#include "atsc_basic_trellis_encoder.h"
#include "atsc_types.h"

namespace gr {
  namespace dtv {

    class atsc_trellis_encoder_impl : public atsc_trellis_encoder
    {
    private:
      bool debug;

      static const int NCODERS = 12;
      static const int SEGMENT_SIZE = ATSC_MPEG_RS_ENCODED_LENGTH;
      static const int INPUT_SIZE = (SEGMENT_SIZE * 12);
      static const int OUTPUT_SIZE = (ATSC_DATA_SEGMENT_LENGTH * 12);

      void reset();
      void encode(atsc_data_segment out[NCODERS], const atsc_mpeg_packet_rs_encoded in[NCODERS]);
      void encode_helper(unsigned char output[OUTPUT_SIZE], const unsigned char input[INPUT_SIZE]);

      atsc_basic_trellis_encoder enc[NCODERS];

    public:
      atsc_trellis_encoder_impl();
      ~atsc_trellis_encoder_impl();

      int work(int noutput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items);
    };

  } // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_ATSC_TRELLIS_ENCODER_IMPL_H */
