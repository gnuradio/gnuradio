/* -*- c++ -*- */
/*
 * Copyright 2013-2014 Free Software Foundation, Inc.
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

#include "depuncture_bb_impl.h"
#include <gnuradio/io_signature.h>
#include <boost/bind.hpp>
#include <pmt/pmt.h>
#include <string>
#include <stdio.h>

namespace gr {
  namespace fec {

    depuncture_bb::sptr
    depuncture_bb::make(int delay, int puncpat, int puncholes, int puncsize)
    {
      return gnuradio::get_initial_sptr
        (new depuncture_bb_impl(delay, puncpat, puncholes, puncsize));
    }

    depuncture_bb_impl::depuncture_bb_impl(int delay, int puncpat,
                                           int puncholes, int puncsize)
      : block("depuncture_bb",
              io_signature::make(1, 1, sizeof(unsigned char)),
              io_signature::make(1, 1, sizeof(unsigned char))),
        d_delay(delay), d_puncholes(puncholes), d_puncsize(puncsize)
    {
      for(int i = 0; i < d_delay; ++i) {
	puncpat = ((puncpat & 1) << (d_puncsize - 1)) + (puncpat >> 1);
      }
      d_puncpat = puncpat;

      set_fixed_rate(true);
      set_relative_rate((double)d_puncsize/(d_puncsize - d_puncholes));
      set_output_multiple(d_puncsize);
      //set_msg_handler(boost::bind(&depuncture_bb_impl::catch_msg, this, _1));
    }

    depuncture_bb_impl::~depuncture_bb_impl()
    {
    }

    int
    depuncture_bb_impl::fixed_rate_ninput_to_noutput(int ninput)
    {
      return (int)(((d_puncsize/(double)(d_puncsize - d_puncholes)) * ninput) + .5);
    }

    int
    depuncture_bb_impl::fixed_rate_noutput_to_ninput(int noutput)
    {
      return (int)((((d_puncsize - d_puncholes)/(double)(d_puncsize)) * noutput) + .5);
    }

    void
    depuncture_bb_impl::forecast(int noutput_items,
                                 gr_vector_int& ninput_items_required)
    {
      ninput_items_required[0] = (int)((((d_puncsize - d_puncholes)/(double)(d_puncsize)) * noutput_items) + .5);
    }

    /*
    void depuncture_bb_impl::catch_msg(pmt::pmt_t msg)
    {
      long mlong = pmt::pmt_to_long(msg);
      for(int i = 0; i < mlong; ++i) {
        d_puncholes = (d_puncholes >> 1) | ((d_puncholes & 1) << (d_puncsize - 1));
      }
    }
    */

    int
    depuncture_bb_impl::general_work(int noutput_items,
                                     gr_vector_int& ninput_items,
                                     gr_vector_const_void_star &input_items,
                                     gr_vector_void_star &output_items)
    {
      const uint8_t *in = (const uint8_t*)input_items[0];
      uint8_t *out = (uint8_t*)output_items[0];

      for(int i=0, k=0; i < noutput_items/output_multiple(); ++i) {
        for(int j = 0; j < output_multiple(); ++j) {
	  out[i*output_multiple() + j] = ((d_puncpat >> (d_puncsize - 1 - j)) & 1) ? in[k++] : 127;
        }
      }

      /*
      printf(">>>>>>start\n");
      for(int i = 0, k=0; i < noutput_items; ++i) {
        if((d_puncpat >> (d_puncsize - 1 - (i % d_puncsize))) & 1) {
	  printf("%d...%d\n", out[i], in[k++]);
        }
        else {
	  printf("snit %d\n", out[i]);
        }
      }
      */

      /*printf("comp: %d, %d\n", noutput_items, ninput_items[0]);
      printf("consuming %d\n", (int)(((1.0/relative_rate()) * noutput_items) + .5));*/

      consume_each((int)(((1.0/relative_rate()) * noutput_items) + .5));
      return noutput_items;
    }

  } /* namespace fec */
} /* namespace gr */


