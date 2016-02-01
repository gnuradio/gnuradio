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

#include <ldpc_bit_flip_decoder_impl.h>
#include <math.h>
#include <boost/assign/list_of.hpp>
#include <volk/volk.h>
#include <sstream>
#include <stdio.h>
#include <vector>

namespace gr {
  namespace fec {
    namespace code {

      generic_decoder::sptr
      ldpc_bit_flip_decoder::make(const fec_mtrx_sptr mtrx_obj,
                                  unsigned int max_iter)
      {
        return generic_decoder::sptr
          (new ldpc_bit_flip_decoder_impl(mtrx_obj, max_iter));
      }

      ldpc_bit_flip_decoder_impl::ldpc_bit_flip_decoder_impl(const fec_mtrx_sptr mtrx_obj,
                                                             unsigned int max_iter)
        : generic_decoder("ldpc_bit_flip_decoder")
      {
        // FEC matrix object to use for decoding
        d_mtrx = mtrx_obj;

        d_rate = static_cast<double>(d_mtrx->k())/static_cast<double>(d_mtrx->n());

        // Set frame size to k, the # of bits in the information word
        // All buffers and settings will be based on this value.
        set_frame_size(d_mtrx->k());
        // Maximum number of iterations in the decoding algorithm
        d_max_iterations = max_iter;
      }

      ldpc_bit_flip_decoder_impl::~ldpc_bit_flip_decoder_impl()
      {
      }

      int
      ldpc_bit_flip_decoder_impl::get_output_size()
      {
        return d_output_size;
      }

      int
      ldpc_bit_flip_decoder_impl::get_input_size()
      {
        return d_input_size;
      }

      bool
      ldpc_bit_flip_decoder_impl::set_frame_size(unsigned int frame_size)
      {
        if(frame_size % d_mtrx->k() != 0) {
          GR_LOG_ERROR(d_logger, boost::format("Frame size (%1% bits) must be a "
                                               "multiple of the information word "
                                               "size of the LDPC matrix, %2%") \
                       % frame_size % (d_mtrx->k()));
          throw std::runtime_error("ldpc_bit_flip_decoder: cannot use frame size.");
        }

        d_output_size = frame_size;
        d_input_size = static_cast<int>(round(frame_size / d_rate));

        return true;
      }

      double
      ldpc_bit_flip_decoder_impl::rate()
      {
        return d_rate;
      }


      void
      ldpc_bit_flip_decoder_impl::generic_work(void *inbuffer,
                                               void *outbuffer)
      {
        // Populate the information word
        const float *in = (const float*)inbuffer;
        unsigned char *out = (unsigned char*) outbuffer;

        int j = 0;
        for(int i = 0; i < d_input_size; i+=d_mtrx->n()) {
          d_mtrx->decode(&out[j], &in[i], d_mtrx->k(), d_max_iterations);
          j += d_mtrx->k();
        }

      } /* ldpc_bit_flip_decoder_impl::generic_work() */

    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */
