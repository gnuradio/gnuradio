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
#include "dvbt_convolutional_deinterleaver_impl.h"

namespace gr {
  namespace dtv {

    const int dvbt_convolutional_deinterleaver_impl::d_SYNC = 0x47;
    const int dvbt_convolutional_deinterleaver_impl::d_NSYNC = 0xB8;
    const int dvbt_convolutional_deinterleaver_impl::d_MUX_PKT = 8;

    dvbt_convolutional_deinterleaver::sptr
    dvbt_convolutional_deinterleaver::make(int nsize, int I, int M)
    {
      return gnuradio::get_initial_sptr
        (new dvbt_convolutional_deinterleaver_impl(nsize, I, M));
    }

    /*
     * The private constructor
     */
    dvbt_convolutional_deinterleaver_impl::dvbt_convolutional_deinterleaver_impl(int blocks, int I, int M)
      : block("dvbt_convolutional_deinterleaver",
          io_signature::make(1, 1, sizeof (unsigned char)),
          io_signature::make(1, 1, sizeof (unsigned char) * I * blocks)),
      d_blocks(blocks), d_I(I), d_M(M)
    {
      set_relative_rate(1.0 / d_I * d_blocks);
      set_output_multiple(2);
      //The positions are shift registers (FIFOs)
      //of length i*M
      for (int i = (d_I - 1); i >= 0; i--) {
        d_shift.push_back(new std::deque<unsigned char>(d_M * i, 0));
      }

      // There are 8 mux packets
      assert(d_blocks / d_M == d_MUX_PKT);
    }

    /*
     * Our virtual destructor.
     */
    dvbt_convolutional_deinterleaver_impl::~dvbt_convolutional_deinterleaver_impl()
    {
      for (unsigned int i = 0; i < d_shift.size(); i++) {
        delete d_shift.back();
        d_shift.pop_back();
      }
    }

    void
    dvbt_convolutional_deinterleaver_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      int ninputs = ninput_items_required.size ();

      for (int i = 0; i < ninputs; i++) {
        ninput_items_required[i] = noutput_items * d_I * d_blocks;
      }
    }


    int
    dvbt_convolutional_deinterleaver_impl::general_work(int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char *) input_items[0];
      unsigned char *out = (unsigned char *) output_items[0];

      int to_out = noutput_items;

      /*
       * Look for a tag that signals superframe_start and consume all input items
       * that are in input buffer so far.
       * This will actually reset the convolutional deinterleaver
       */
      std::vector<tag_t> tags;
      const uint64_t nread = this->nitems_read(0); //number of items read on port 0
      this->get_tags_in_range(tags, 0, nread, nread + (noutput_items * d_I * d_blocks), pmt::string_to_symbol("superframe_start"));

      if (tags.size()) {
        if (tags[0].offset - nread) {
          consume_each(tags[0].offset - nread);
          return (0);
        }
      }

      /*
       * At this moment the first item in input buffer should be NSYNC or SYNC
       */

      for (int count = 0, i = 0; i < to_out; i++) {
        for (int mux_pkt = 0; mux_pkt < d_MUX_PKT; mux_pkt++) {
          // This is actually the deinterleaver
          for (int k = 0; k < (d_M * d_I); k++) {
            d_shift[k % d_I]->push_back(in[count]);
            out[count++] = d_shift[k % d_I]->front();
            d_shift[k % d_I]->pop_front();
          }
        }
      }

      // Tell runtime system how many input items we consumed on
      // each input stream.
      consume_each(d_I * d_blocks * to_out);

      // Tell runtime system how many output items we produced.
      return (to_out);
    }

  } /* namespace dtv */
} /* namespace gr */

