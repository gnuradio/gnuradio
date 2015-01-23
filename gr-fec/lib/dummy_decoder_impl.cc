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

#include "dummy_decoder_impl.h"
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
      dummy_decoder::make(int frame_size)
      {
        return generic_decoder::sptr
          (new dummy_decoder_impl(frame_size));
      }

      dummy_decoder_impl::dummy_decoder_impl(int frame_size)
        : generic_decoder("dummy_decoder")
      {
        // Set max frame size here; all buffers and settings will be
        // based on this value.
        d_max_frame_size = frame_size;
        set_frame_size(frame_size);
      }

      dummy_decoder_impl::~dummy_decoder_impl()
      {
      }

      int
      dummy_decoder_impl::get_output_size()
      {
        //unpacked bits
        return d_frame_size;
      }

      int
      dummy_decoder_impl::get_input_size()
      {
        return d_frame_size;
      }

      int
      dummy_decoder_impl::get_input_item_size()
      {
        return sizeof(float);
      }

      const char*
      dummy_decoder_impl::get_input_conversion()
      {
        return "none";
      }

      bool
      dummy_decoder_impl::set_frame_size(unsigned int frame_size)
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
      dummy_decoder_impl::rate()
      {
        return 1.0;
      }

      void
      dummy_decoder_impl::generic_work(void *inbuffer, void *outbuffer)
      {
        const float *in = (const float*)inbuffer;
        int8_t *out = (int8_t*)outbuffer;

        //memcpy(out, in, d_frame_size*sizeof(char));
        volk_32f_binary_slicer_8i(out, in, d_frame_size);
        //volk_32f_s32f_convert_8i(out, in, 1.0/2.0, d_frame_size);
      }

    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */
