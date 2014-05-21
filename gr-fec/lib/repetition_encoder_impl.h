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

#ifndef INCLUDED_FEC_REPETITION_ENCODER_IMPL_H
#define INCLUDED_FEC_REPETITION_ENCODER_IMPL_H

#include <map>
#include <string>
#include <gnuradio/fec/repetition_encoder.h>

namespace gr {
  namespace fec {
    namespace code {

      class FEC_API repetition_encoder_impl : public repetition_encoder
      {
      private:
        //plug into the generic fec api
        void generic_work(void *inbuffer, void *outbuffer);
        int get_output_size();
        int get_input_size();

        unsigned int d_max_frame_size;
        unsigned int d_frame_size;
        unsigned int d_rep;

      public:
        repetition_encoder_impl(int frame_size, int rep);
        ~repetition_encoder_impl();

        bool set_frame_size(unsigned int frame_size);
        double rate();
      };

    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_REPETITION_ENCODER_IMPL_H */
