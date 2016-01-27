/* -*- c++ -*- */
/* 
 * Copyright 2016 Free Software Foundation, Inc.
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
#include "catv_frame_sync_enc_bb_impl.h"

namespace gr {
  namespace dtv {

    catv_frame_sync_enc_bb::sptr
    catv_frame_sync_enc_bb::make(int ctrlword)
    {
      return gnuradio::get_initial_sptr
        (new catv_frame_sync_enc_bb_impl(ctrlword));
    }

    /*
     * The private constructor
     */
    catv_frame_sync_enc_bb_impl::catv_frame_sync_enc_bb_impl(int ctrlword)
      : gr::block("catv_frame_sync_enc_bb",
              gr::io_signature::make(1, 1, sizeof(unsigned char)),
              gr::io_signature::make(1, 1, sizeof(unsigned char)))
    {
      set_output_multiple(60 * 128 + 6);
      control_word = ctrlword;
    }

    /*
     * Our virtual destructor.
     */
    catv_frame_sync_enc_bb_impl::~catv_frame_sync_enc_bb_impl()
    {
    }

    void
    catv_frame_sync_enc_bb_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = noutput_items / (60 * 128 + 6) * (60 * 128);
    }

    int
    catv_frame_sync_enc_bb_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char *) input_items[0];
      unsigned char *out = (unsigned char *) output_items[0];

      int i = 0, j = 0;
      while (i < noutput_items) {
        memcpy(out + i, in + j, 60 * 128);
        i += 60 * 128;
        j += 60 * 128;

        out[i++] = 0x75;
        out[i++] = 0x2C;
        out[i++] = 0x0D;
        out[i++] = 0x6C;
        out[i++] = control_word << 3;
        out[i++] = 0x00;
      }

      // Tell runtime system how many input items we consumed on
      // each input stream.
      consume_each (j);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace dtv */
} /* namespace gr */

