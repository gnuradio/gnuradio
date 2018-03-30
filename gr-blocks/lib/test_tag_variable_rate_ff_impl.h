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

#ifndef INCLUDED_GR_TEST_TAG_VARIABLE_RATE_FF_IMPL_H
#define INCLUDED_GR_TEST_TAG_VARIABLE_RATE_FF_IMPL_H

#include <gnuradio/blocks/test_tag_variable_rate_ff.h>

namespace gr {
  namespace blocks {

    class test_tag_variable_rate_ff_impl : public test_tag_variable_rate_ff
    {
    protected:
      bool d_update_once;
      double d_update_step;
      double d_accum;
      double d_rrate;
      uint64_t d_old_in, d_new_in, d_last_out;
      uint64_t d_rng_state[2];

    public:
      test_tag_variable_rate_ff_impl(bool update_once=false,
                                     double update_step=0.001);
      ~test_tag_variable_rate_ff_impl();

      //void setup_rpc();

      int general_work(int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_TEST_TAG_VARIABLE_RATE_FF_IMPL_H */
