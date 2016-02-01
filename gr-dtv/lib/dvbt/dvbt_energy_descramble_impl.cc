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
#include "dvbt_energy_descramble_impl.h"

namespace gr {
  namespace dtv {

    const int dvbt_energy_descramble_impl::d_nblocks = 8;
    const int dvbt_energy_descramble_impl::d_bsize = 188;
    const int dvbt_energy_descramble_impl::d_SYNC = 0x47;
    const int dvbt_energy_descramble_impl::d_NSYNC = 0xB8;
    const int dvbt_energy_descramble_impl::d_MUX_PKT = 8;

    void
    dvbt_energy_descramble_impl::init_prbs()
    {
      d_reg = 0xa9;
    }

    int
    dvbt_energy_descramble_impl::clock_prbs(int clocks)
    {
      int res = 0;
      int feedback = 0;

      for(int i = 0; i < clocks; i++) {
        feedback = ((d_reg >> (14 - 1)) ^ (d_reg >> (15 - 1))) & 0x1;
        d_reg = ((d_reg << 1) | feedback) & 0x7fff;

        res = (res << 1) | feedback;
      }

      return res;
    }

    dvbt_energy_descramble::sptr
    dvbt_energy_descramble::make(int nblocks)
    {
      return gnuradio::get_initial_sptr
        (new dvbt_energy_descramble_impl(nblocks));
    }

    /*
     * The private constructor
     */
    dvbt_energy_descramble_impl::dvbt_energy_descramble_impl(int nblocks)
      : block("dvbt_energy_descramble",
          io_signature::make(1, 1, sizeof (unsigned char) * d_nblocks * d_bsize),
          io_signature::make(1, 1, sizeof (unsigned char))),
      d_reg(0xa9), d_index(0), d_search(0)
    {
      set_relative_rate((double) (d_nblocks * d_bsize));

      // Set output multiple of (2 search + 1 data = 3) at least (4 for now)
      set_output_multiple(4 * d_nblocks * d_bsize);

      // Search interval for NSYNC is 2 * 8 * MUX size
      d_search = 2 * (d_nblocks * d_bsize);
    }

    /*
     * Our virtual destructor.
     */
    dvbt_energy_descramble_impl::~dvbt_energy_descramble_impl()
    {
    }

    void
    dvbt_energy_descramble_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = 4 * (noutput_items / (d_nblocks * d_bsize));
    }

    int
    dvbt_energy_descramble_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char *) input_items[0];
      unsigned char *out = (unsigned char *) output_items[0];

      int to_consume, to_out;

      // Search for a NSYNC
      while ((in[d_index] != d_NSYNC) && (d_index < d_search)) {
        d_index += d_bsize;
      }

      /*
       * If we did not find a NSYNC then just consume
       * and return 0.
       */
      if (d_index >= d_search) {
        d_index = 0;
        to_consume = 2;
        to_out = 0;
      }
      else {
        // We found a NSYNC, descramble the data

        to_consume = (noutput_items / (d_nblocks * d_bsize)) - 2;
        to_out = noutput_items - 2 * (d_nblocks * d_bsize);

        for (int count = 0, i = 0; i < to_consume; i++) {
          init_prbs();

          for (int mux_pkt = 0; mux_pkt < d_MUX_PKT; mux_pkt++) {
            out[count++] = d_SYNC;
            // PRBS clocking starts right after NSYNC

            for (int k = 1; k < d_bsize; k++) {
              out[count] = in[d_index + count] ^ clock_prbs(d_nblocks);
              count++;
            }

            // For subsequent blocks PRBS is clocked also on SYNC
            // but its output is not used
            clock_prbs(d_nblocks);
          }
        }
      }

      // Tell runtime how many input items we consumed
      consume_each(to_consume);

      // Tell runtime system how many output items we produced.
      return (to_out);
    }

  } /* namespace dtv */
} /* namespace gr */

