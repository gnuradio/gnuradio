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
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "repetition_encoder_impl.h"
#include <gnuradio/fec/generic_encoder.h>
#include <volk/volk.h>
#include <sstream>

namespace gr {
  namespace fec {
    namespace code {

      generic_encoder::sptr
      repetition_encoder::make(int frame_size, int rep)
      {
        return generic_encoder::sptr
          (new repetition_encoder_impl(frame_size, rep));
      }

      repetition_encoder_impl::repetition_encoder_impl(int frame_size, int rep)
        : generic_encoder("repetition_encoder")
      {
        d_max_frame_size = frame_size;
        set_frame_size(frame_size);

        if(rep < 0)
          throw std::runtime_error("repetition_encoder: repetition rate must be >= 0");

        d_rep = rep;
      }

      repetition_encoder_impl::~repetition_encoder_impl()
      {
      }

      int
      repetition_encoder_impl::get_output_size()
      {
        return d_frame_size*d_rep;
      }

      int
      repetition_encoder_impl::get_input_size()
      {
        return d_frame_size;
      }

      bool
      repetition_encoder_impl::set_frame_size(unsigned int frame_size)
      {
        bool ret = true;
        if(frame_size > d_max_frame_size) {
          GR_LOG_INFO(d_logger, boost::format("tried to set frame to %1%; max possible is %2%") \
                      % frame_size % d_max_frame_size);
          frame_size = d_max_frame_size;
          ret = false;
        }

        d_frame_size = frame_size;

        return ret;
      }

      double
      repetition_encoder_impl::rate()
      {
        return static_cast<double>(d_rep);
      }

      void
      repetition_encoder_impl::generic_work(void *inbuffer, void *outbuffer)
      {
        const unsigned char *in = (const unsigned char*)inbuffer;
        unsigned char *out = (unsigned char*)outbuffer;

        for(unsigned int i = 0; i < d_frame_size; i++) {
          for(unsigned int r = 0; r < d_rep; r++) {
            out[d_rep*i + r] = in[i];
          }
        }
      }

    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */
