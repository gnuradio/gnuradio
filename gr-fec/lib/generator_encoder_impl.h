/* -*- c++ -*- */
/* 
 * Copyright 2013-2014 Free Software Foundation, Inc.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published 
 * by the Free Software Foundation; either version 3, or (at your 
 * option) any later version.
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

#ifndef INCLUDED_FEC_GENERATOR_ENCODER_IMPL_H
#define INCLUDED_FEC_GENERATOR_ENCODER_IMPL_H

#include <map>
#include <string>
#include <gnuradio/fec/generator_encoder.h>
#include <gnuradio/fec/generator_mtrx.h>

namespace gr {
  namespace fec {
    namespace code {
      class FEC_API generator_encoder_impl : public generator_encoder
      {
      private:
        void generic_work(void *inbuffer, void *outbuffer);
        int get_output_size();
        int get_input_size();

        // Number of bits in the information word
        unsigned int d_frame_size;
        // Generator matrix object to use for encoding
        generator_mtrx *d_G;

      public:
        generator_encoder_impl(generator_mtrx *G_obj);
        ~generator_encoder_impl();

        bool set_frame_size(unsigned int frame_size);
        double rate();
      };
    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_GENERATOR_ENCODER_IMPL_H */