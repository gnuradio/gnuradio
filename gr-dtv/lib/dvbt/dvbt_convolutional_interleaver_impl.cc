/* -*- c++ -*- */
/* 
 * Copyright 2015 Free Software Foundation, Inc.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
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

#include <gnuradio/io_signature.h>
#include "dvbt_convolutional_interleaver_impl.h"
#include <deque>

namespace gr {
  namespace dtv {

    dvbt_convolutional_interleaver::sptr
    dvbt_convolutional_interleaver::make(int nsize, int I, int M)
    {
      return gnuradio::get_initial_sptr
        (new dvbt_convolutional_interleaver_impl(nsize, I, M));
    }

    /*
     * The private constructor
     */
    dvbt_convolutional_interleaver_impl::dvbt_convolutional_interleaver_impl(int blocks, int I, int M)
      : sync_interpolator("dvbt_convolutional_interleaver",
          io_signature::make(1, 1, sizeof (unsigned char) * I * blocks),
          io_signature::make(1, 1, sizeof (unsigned char)), I * blocks),
      d_blocks(blocks), d_I(I), d_M(M)
    {
      //Positions are shift registers (FIFOs)
      //of length i*M
      for (int i = 0; i < d_I; i++) {
        d_shift.push_back(new std::deque<unsigned char>(d_M * i, 0));
      }
    }

    /*
     * Our virtual destructor.
     */
    dvbt_convolutional_interleaver_impl::~dvbt_convolutional_interleaver_impl()
    {
      for (unsigned int i = 0; i < d_shift.size(); i++) {
        delete d_shift.back();
        d_shift.pop_back();
      }
    }

    int
    dvbt_convolutional_interleaver_impl::work (int noutput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char *) input_items[0];
      unsigned char *out = (unsigned char *) output_items[0];

      for (int i = 0; i < (noutput_items / d_I); i++) {
        //Process one block of I symbols
        for (unsigned int j = 0; j < d_shift.size(); j++) {
          d_shift[j]->push_front(in[(d_I * i) + j]);
          out[(d_I * i) + j] = d_shift[j]->back();
          d_shift[j]->pop_back();
        }
      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace dtv */
} /* namespace gr */

