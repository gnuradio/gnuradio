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

#include "decoder_impl.h"
#include <gnuradio/io_signature.h>
#include <stdio.h>

namespace gr {
  namespace fec {

    decoder::sptr
    decoder::make(generic_decoder::sptr my_decoder,
                  size_t input_item_size,
                  size_t output_item_size)
    {
      return gnuradio::get_initial_sptr
        ( new decoder_impl(my_decoder, input_item_size, output_item_size));
    }

    decoder_impl::decoder_impl(generic_decoder::sptr my_decoder,
                               size_t input_item_size,
                               size_t output_item_size)
      : block("fec_decoder",
              io_signature::make(1, 1, input_item_size),
              io_signature::make(1, 1, output_item_size)),
        d_input_item_size(input_item_size), d_output_item_size(output_item_size)
    {
      set_fixed_rate(true);
      set_relative_rate((double)(my_decoder->get_output_size())/my_decoder->get_input_size());

      //want to guarantee you have enough to run at least one time...
      //remember! this is not a sync block... set_output_multiple does not
      //actually guarantee the output multiple... it DOES guarantee how many
      //outputs (hence inputs) are made available... this is WEIRD to do in
      //GNU Radio, and the algorithm is sensitive to this value
      set_output_multiple(my_decoder->get_output_size() + (my_decoder->get_history()));
      d_decoder = my_decoder;
    }

    int
    decoder_impl::fixed_rate_ninput_to_noutput(int ninput)
    {
      return (int)(0.5 + ninput*relative_rate());
    }

    int
    decoder_impl::fixed_rate_noutput_to_ninput(int noutput)
    {
      return (int)(0.5 + noutput/relative_rate());
    }

    void
    decoder_impl::forecast(int noutput_items,
                           gr_vector_int& ninput_items_required)
    {
      ninput_items_required[0] = 0.5 + fixed_rate_noutput_to_ninput(noutput_items);
    }

    int
    decoder_impl::general_work(int noutput_items,
                               gr_vector_int& ninput_items,
                               gr_vector_const_void_star &input_items,
                               gr_vector_void_star &output_items)
    {
      const unsigned char *in = (unsigned char*)input_items[0];
      unsigned char *out = (unsigned char *)output_items[0];

      int outnum = (int)(((1.0/relative_rate()) * noutput_items) + 0.5);
      int innum = (int)(relative_rate() * (ninput_items[0] - d_decoder->get_history()) + 0.5)/(output_multiple() - d_decoder->get_history());

      int items = (outnum <= ninput_items[0] - d_decoder->get_history()) ?
	noutput_items/(output_multiple() - d_decoder->get_history()) :
	innum;

      for(int i = 0; i < items; ++i) {
        d_decoder->generic_work((void*)(in+(i*d_decoder->get_input_size()*d_input_item_size)),
                                (void*)(out+(i*d_decoder->get_output_size()*d_output_item_size)));

        add_item_tag(0, nitems_written(0) + ((i+1)*d_decoder->get_output_size()*d_output_item_size),
                     pmt::intern(d_decoder->alias()), pmt::PMT_T, pmt::intern(alias()));
      }

      int consumed = static_cast<int>(items/relative_rate()*(output_multiple() - d_decoder->get_history()) + 0.5);
      int returned = items*(output_multiple() - d_decoder->get_history());

      consume_each(consumed);
      return returned;
    }

  } /* namespace fec */
} /* namespace gr */
