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

#include "repetition_decoder_impl.h"
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
      repetition_decoder::make(int frame_size, int rep,
                               float ap_prob)
      {
        return generic_decoder::sptr
          (new repetition_decoder_impl(frame_size, rep,
                                       ap_prob));
      }

      repetition_decoder_impl::repetition_decoder_impl(int frame_size, int rep,
                                                       float ap_prob)
        : generic_decoder("repetition_decoder")
      {
        // Set max frame size here; all buffers and settings will be
        // based on this value.
        d_max_frame_size = frame_size;
        set_frame_size(frame_size);

        if(rep < 0)
          throw std::runtime_error("repetition_encoder: repetition rate must be >= 0");
        if((ap_prob < 0) || (ap_prob > 1.0))
          throw std::runtime_error("repetition_encoder: a priori probability rate must be in [0, 1]");

        d_rep = rep;
        d_ap_prob = ap_prob;
        d_trials.resize(d_rep);
      }

      repetition_decoder_impl::~repetition_decoder_impl()
      {
      }

      int
      repetition_decoder_impl::get_output_size()
      {
        //unpacked bits
        return d_frame_size;
      }

      int
      repetition_decoder_impl::get_input_size()
      {
        return d_frame_size*d_rep;
      }

      int
      repetition_decoder_impl::get_input_item_size()
      {
        return sizeof(float);
      }

      const char*
      repetition_decoder_impl::get_input_conversion()
      {
        return "none";
      }

      float
      repetition_decoder_impl::get_shift()
      {
        return 0;
      }

      bool
      repetition_decoder_impl::set_frame_size(unsigned int frame_size)
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
      repetition_decoder_impl::rate()
      {
        return 1.0/static_cast<double>(d_rep);
      }

      void
      repetition_decoder_impl::generic_work(void *inbuffer, void *outbuffer)
      {
        const float *in = (const float*)inbuffer;
        unsigned char *out = (unsigned char *) outbuffer;

        for(unsigned int i = 0; i < d_frame_size; i++) {
          for(unsigned int r = 0; r < d_rep; r++) {
            d_trials[r] = (in[d_rep*i + r] > 0) ? 1.0f : 0.0f;
          }
          float res = std::count(d_trials.begin(), d_trials.end(), 1.0f);
          if((res / static_cast<float>(d_rep)) > d_ap_prob)
            out[i] = 1;
          else
            out[i] = 0;
        }
      }

    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */
