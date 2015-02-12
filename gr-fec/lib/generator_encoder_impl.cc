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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <generator_encoder_impl.h>
#include <sstream>

namespace gr {
  namespace fec {
    namespace code {
      generic_encoder::sptr
      generator_encoder::make(generator_mtrx *G_obj)
      {
        return generic_encoder::sptr
          (new generator_encoder_impl(G_obj));
      }

      generator_encoder_impl::generator_encoder_impl(generator_mtrx *G_obj) 
        : generic_encoder("generator_encoder")
      {
        // Generator matrix to use for encoding
        d_G = G_obj;
        // Set frame size to k, the # of bits in the information word
        // All buffers and settings will be based on this value.
        set_frame_size(d_G->k());
      }

      generator_encoder_impl::~generator_encoder_impl()
      {
      }

      int
      generator_encoder_impl::get_output_size()
      {
        return d_G->n();
      }

      int
      generator_encoder_impl::get_input_size()
      {
        return d_frame_size;
      }

      bool
      generator_encoder_impl::set_frame_size(unsigned int frame_size)
      {
        bool ret = true;
        // TODO add some bounds check here? The frame size is
        // constant and specified by the size of the parity check
        // matrix used for encoding.
        d_frame_size = frame_size;

        return ret;        
      }

      double
      generator_encoder_impl::rate()
      {
        return (d_G->n())/static_cast<double>(d_frame_size);
      }

      void
      generator_encoder_impl::generic_work(void *inbuffer,
                                           void *outbuffer)
      {


        // Free memory


      }
    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */