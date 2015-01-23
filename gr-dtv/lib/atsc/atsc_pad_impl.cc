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

#include "atsc_pad_impl.h"
#include "gnuradio/dtv/atsc_consts.h"

namespace gr {
  namespace dtv {

    atsc_pad::sptr
    atsc_pad::make()
    {
      return gnuradio::get_initial_sptr
        (new atsc_pad_impl());
    }

    atsc_pad_impl::atsc_pad_impl()
      : gr::sync_decimator("atsc_pad",
                           gr::io_signature::make(1, 1, sizeof(unsigned char)),
                           gr::io_signature::make(1, 1, sizeof(atsc_mpeg_packet)), ATSC_MPEG_PKT_LENGTH)
    {
    }

    atsc_pad_impl::~atsc_pad_impl()
    {
    }

    int
    atsc_pad_impl::work(int noutput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char *) input_items[0];
      atsc_mpeg_packet *out = (atsc_mpeg_packet *) output_items[0];

      for (int i = 0; i < noutput_items; i++) {
        memcpy(out[i].data, &in[i * ATSC_MPEG_PKT_LENGTH], ATSC_MPEG_PKT_LENGTH);
      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace dtv */
} /* namespace gr */
