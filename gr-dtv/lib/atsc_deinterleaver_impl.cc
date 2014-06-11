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

#include "atsc_deinterleaver_impl.h"
#include "gnuradio/dtv/atsc_consts.h"
#include <gnuradio/io_signature.h>

namespace gr {
  namespace dtv {

    atsc_deinterleaver::sptr
    atsc_deinterleaver::make()
    {
      return gnuradio::get_initial_sptr
	(new atsc_deinterleaver_impl());
    }

    atsc_deinterleaver_impl::atsc_deinterleaver_impl()
      : gr::sync_block("atsc_deinterleaver",
                       io_signature::make(1, 1, sizeof(atsc_mpeg_packet_rs_encoded)),
                       io_signature::make(1, 1, sizeof(atsc_mpeg_packet_rs_encoded))),
        alignment_fifo (156)
    {
      m_fifo.resize(52);

      for (int i = 0; i < 52; i++)
        m_fifo[52 - 1 - i] = new interleaver_fifo<unsigned char>(i * 4);

      sync();
    }

    atsc_deinterleaver_impl::~atsc_deinterleaver_impl()
    {
      for (int i = 0; i < 52; i++)
        delete m_fifo[i];
    }

    void atsc_deinterleaver_impl::reset()
    {
      sync();
      for (int i = 0; i < 52; i++)
        m_fifo[i]->reset();
    }

    int
    atsc_deinterleaver_impl::work(int noutput_items,
                                  gr_vector_const_void_star &input_items,
                                  gr_vector_void_star &output_items)
    {
      const atsc_mpeg_packet_rs_encoded *in = (const atsc_mpeg_packet_rs_encoded *)input_items[0];
      atsc_mpeg_packet_rs_encoded *out = (atsc_mpeg_packet_rs_encoded *)output_items[0];

      for (int i = 0; i < noutput_items; i++) {
        assert (in[i].pli.regular_seg_p());
        plinfo::sanity_check(in[i].pli);

        // reset commutator if required using INPUT pipeline info
        if (in[i].pli.first_regular_seg_p())
          sync();

        // remap OUTPUT pipeline info to reflect 52 data segment end-to-end delay
        plinfo::delay (out[i].pli, in[i].pli, 52);

        // now do the actual deinterleaving
        for (unsigned int j = 0; j < sizeof(in[i].data); j++) {
          out[i].data[j] = alignment_fifo.stuff(transform (in[i].data[j]));
        }
      }

      return noutput_items;
    }

  } /* namespace dtv */
} /* namespace gr */
