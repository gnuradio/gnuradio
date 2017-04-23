/* -*- c++ -*- */
/* 
 * Copyright 2017 Free Software Foundation, Inc.
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

#include <gnuradio/io_signature.h>
#include <gnuradio/buffer.h>
#include <boost/thread.hpp>
#include "concatenate_impl.h"

namespace gr {
  namespace blocks {

    concatenate::sptr
    concatenate::make(size_t itemsize, int ninputs)
    {
      return gnuradio::get_initial_sptr
        (new concatenate_impl(itemsize, ninputs));
    }

    /*
     * The private constructor
     */
    concatenate_impl::concatenate_impl(size_t itemsize, int ninputs)
      : gr::block("concatenate",
              gr::io_signature::make(ninputs, ninputs, itemsize),
              gr::io_signature::make(1, 1, itemsize)),
        d_itemsize(itemsize), d_ninputs(ninputs), d_current_done(false),
        d_current_input(0)
    {

      set_tag_propagation_policy(TPP_DONT);
    }

    /*
     * Our virtual destructor.
     */
    concatenate_impl::~concatenate_impl()
    {
    }

    bool
    concatenate_impl::start()
    {
      d_block_detail = this->detail().get();
      return true;
    }

    void
    concatenate_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      int d_ninput_items;
      bool d_input_done;
      int current_ninput_items_required;

      current_ninput_items_required = noutput_items;

      if (d_current_input >= 0 && d_current_input < d_ninputs) {
        gr::thread::scoped_lock guard(*d_block_detail->input(d_current_input)->mutex());
        d_ninput_items = d_block_detail->input(d_current_input)->items_available ();
        d_input_done = d_block_detail->input(d_current_input)->done();
        if (d_input_done) {
          d_current_done = true;
          d_current_samples_left = d_ninput_items;
          current_ninput_items_required = 0;
        }
      }

      for(int i=0; i < d_ninputs; i++) {
        if (i == d_current_input)
          ninput_items_required[i] = current_ninput_items_required;
        else
          ninput_items_required[i] = 0;
      }
    }

    int
    concatenate_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      int minval;
      int produced = 0;

      const char *in;
      if (d_current_input >= 0)
        in = (const char *) input_items[d_current_input];
      char *out = (char *) output_items[0];

      if (d_current_input == -1)
        return -1;

      minval = std::min(ninput_items[d_current_input], noutput_items);
      memcpy(out, in, d_itemsize * minval);
      produced += minval;

      // Tell runtime system how many input items we consumed.
      // We only consume from one input at a time.
      consume (d_current_input, produced);

      // If the current input stream has stopped producing items,
      // check if we have consumed as many as there were left
      // when forecast was called. In that case, go to the
      // next input.
      if (d_current_done) {
        if(d_current_samples_left == produced) {
          d_current_input++;
          d_current_done = false;
          if (d_current_input >= d_ninputs) {
            d_current_input = -1;
          }
        }
      }

      // Tell runtime system how many output items we produced.
      return produced;
    }

  } /* namespace blocks */
} /* namespace gr */

