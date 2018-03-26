/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
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

#include "test_tag_variable_rate_ff_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/xoroshiro128p.h>
#include <string.h>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <stdint.h>

using namespace pmt;

namespace gr {
  namespace blocks {

    test_tag_variable_rate_ff::sptr
    test_tag_variable_rate_ff::make(bool update_once, double update_step)
    {
      return gnuradio::get_initial_sptr
        (new test_tag_variable_rate_ff_impl(update_once, update_step));
    }

    test_tag_variable_rate_ff_impl::test_tag_variable_rate_ff_impl(bool update_once, double update_step)
      : block("test_tag_variable_rate_ff",
              io_signature::make(1, 1, sizeof(float)),
              io_signature::make(1, 1, sizeof(float))),
      d_update_once(update_once), d_update_step(update_step)
    {
      set_tag_propagation_policy(TPP_DONT);
      set_relative_rate(0.5);
      d_accum = 0;
      d_rrate = 0.5;
      d_old_in = 0;
      d_new_in = 0;
      d_last_out = 0;

      xoroshiro128p_seed(d_rng_state, 4 /* chosen by fair dice roll */);
    }

    test_tag_variable_rate_ff_impl::~test_tag_variable_rate_ff_impl()
    {
    }

    int
    test_tag_variable_rate_ff_impl::general_work(int noutput_items,
                                                 gr_vector_int &ninput_items,
                                                 gr_vector_const_void_star &input_items,
                                                 gr_vector_void_star &output_items)
    {
      const float *in = (const float*)input_items[0];
      float *out = (float*)output_items[0];

      GR_LOG_DEBUG(d_logger, "\n");
      GR_LOG_DEBUG(d_logger, boost::format("ninput_items:  %1%") % ninput_items[0]);
      GR_LOG_DEBUG(d_logger, boost::format("noutput_items: %1%") % noutput_items);

      if(d_update_once) {
        if (xoroshiro128p_next(d_rng_state) > (UINT64_MAX / 2)){
          d_rrate += d_update_step;
        }
        else {
          d_rrate -= d_update_step;
          if(d_rrate < d_update_step)
            d_rrate = d_update_step;
        }
      }

      std::vector<tag_t> tags;
      std::vector<tag_t>::iterator itags;

      int i = 0, j = 0;
      while(i < ninput_items[0]) {

        if(!d_update_once) {
          if (xoroshiro128p_next(d_rng_state) > (UINT64_MAX / 2)){
            d_rrate += d_update_step;
          }
          else {
            d_rrate -= d_update_step;
            if(d_rrate < d_update_step)
              d_rrate = d_update_step;
          }
        }

        d_accum += d_rrate;
        if(d_accum >= 1.0) {
          out[j] = in[i];

          // Manage Tags
          d_new_in = nitems_read(0) + i;
          get_tags_in_range(tags, 0, d_old_in, d_new_in);
          for(itags = tags.begin(); itags != tags.end(); itags++) {
            tag_t new_tag = *itags;
            new_tag.offset = d_last_out;
            add_item_tag(0, new_tag);
          }
          d_old_in = d_new_in;
          d_last_out = nitems_written(0) + j;

          // Book keeping
          j++;
          d_accum -= 1.0;
        }
        i++;
      }

      GR_LOG_DEBUG(d_logger, boost::format("consuming: %1%") % i);
      GR_LOG_DEBUG(d_logger, boost::format("producing: %1%") % j);
      GR_LOG_DEBUG(d_logger, boost::format("block's rel rate:  %1%") % d_rrate);

      consume_each(i);
      return j;
    }

  } /* namespace blocks */
} /* namespace gr */
