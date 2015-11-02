/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
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
p * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_FEC_DUMMY_ENCODER_IMPL_H
#define INCLUDED_FEC_DUMMY_ENCODER_IMPL_H

#include <map>
#include <string>
#include <gnuradio/fec/dummy_encoder.h>

namespace gr {
  namespace fec {
    namespace code {

      class FEC_API dummy_encoder_impl : public dummy_encoder
      {
      private:
        //plug into the generic fec api
        void generic_work(void *inbuffer, void *outbuffer);
        int get_output_size();
        int get_input_size();
        const char* get_input_conversion();
        const char* get_output_conversion();
        bool d_pack_input;
        bool d_packed_bits_output;

        unsigned int d_max_frame_size;
        unsigned int d_frame_size;

      public:
        dummy_encoder_impl(int frame_size, bool pack = false, bool packed_bits = false);
        ~dummy_encoder_impl();

        bool set_frame_size(unsigned int frame_size);
        double rate();
      };

    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_DUMMY_ENCODER_IMPL_H */
