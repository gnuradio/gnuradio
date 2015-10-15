/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
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

#include <ldpc_gen_mtrx_encoder_impl.h>
#include <sstream>

namespace gr {
  namespace fec {
    namespace code {

      generic_encoder::sptr
      ldpc_gen_mtrx_encoder::make(const ldpc_G_matrix::sptr G_obj)
      {
        return generic_encoder::sptr
          (new ldpc_gen_mtrx_encoder_impl(G_obj));
      }

      ldpc_gen_mtrx_encoder_impl::ldpc_gen_mtrx_encoder_impl(const ldpc_G_matrix::sptr G_obj)
        : generic_encoder("ldpc_gen_mtrx_encoder")
      {
        // Generator matrix to use for encoding
        d_G = G_obj;

        d_rate = static_cast<double>(d_G->n())/static_cast<double>(d_G->k());

        // Set frame size to k, the # of bits in the information word
        // All buffers and settings will be based on this value.
        set_frame_size(d_G->k());
      }

      ldpc_gen_mtrx_encoder_impl::~ldpc_gen_mtrx_encoder_impl()
      {
      }

      int
      ldpc_gen_mtrx_encoder_impl::get_output_size()
      {
        return d_output_size;
      }

      int
      ldpc_gen_mtrx_encoder_impl::get_input_size()
      {
        return d_frame_size;
      }

      bool
      ldpc_gen_mtrx_encoder_impl::set_frame_size(unsigned int frame_size)
      {
        bool ret = true;

        if(frame_size % d_G->k() != 0) {
          GR_LOG_ERROR(d_logger, boost::format("Frame size (%1% bits) must be a "
                                               "multiple of the information word "
                                               "size of the LDPC matrix (%2%).") \
                       % frame_size % (d_G->k()));
          throw std::runtime_error("ldpc_gen_mtrx_encoder: cannot use frame size.");
        }

        d_frame_size = frame_size;

        d_output_size = static_cast<int>(d_rate * d_frame_size);

        return ret;
      }

      double
      ldpc_gen_mtrx_encoder_impl::rate()
      {
        return d_rate;
      }

      void
      ldpc_gen_mtrx_encoder_impl::generic_work(void *inbuffer,
                                               void *outbuffer)
      {
        // Populate the information word
        const unsigned char *in = (const unsigned char *)inbuffer;
        unsigned char *out = (unsigned char*)outbuffer;

        int j = 0;
        for(int i = 0; i < get_input_size(); i+=d_G->k()) {
          d_G->encode(&out[j], &in[i]);
          j += d_G->n();
        }
      }

    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */
