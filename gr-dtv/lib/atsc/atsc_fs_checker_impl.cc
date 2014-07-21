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

#include "atsc_fs_checker_impl.h"
#include "atsc_types.h"
#include "atsc_syminfo_impl.h"
#include "atsc_pnXXX_impl.h"
#include "gnuradio/dtv/atsc_consts.h"
#include <gnuradio/io_signature.h>

static const int PN511_ERROR_LIMIT = 20;	// max number of bits wrong
static const int PN63_ERROR_LIMIT = 5;

namespace gr {
  namespace dtv {

    atsc_fs_checker::sptr
    atsc_fs_checker::make()
    {
      return gnuradio::get_initial_sptr
	(new atsc_fs_checker_impl());
    }

    atsc_fs_checker_impl::atsc_fs_checker_impl()
      : gr::block("dtv_atsc_fs_checker",
                  io_signature::make(1, 1, sizeof(atsc_soft_data_segment)),
                  io_signature::make(1, 1, sizeof(atsc_soft_data_segment)))
    {
      reset();
    }

    void
    atsc_fs_checker_impl::reset()
    {
      d_index = 0;
      memset (d_sample_sr, 0, sizeof (d_sample_sr));
      memset (d_tag_sr, 0, sizeof (d_tag_sr));
      memset (d_bit_sr, 0, sizeof (d_bit_sr));
      d_field_num = 0;
      d_segment_num = 0;
    }

    atsc_fs_checker_impl::~atsc_fs_checker_impl()
    {
    }

    int
    atsc_fs_checker_impl::general_work(int noutput_items,
                                 gr_vector_int &ninput_items,
                                 gr_vector_const_void_star &input_items,
                                 gr_vector_void_star &output_items)
    {
      const atsc_soft_data_segment *in = (const atsc_soft_data_segment *) input_items[0];
      atsc_soft_data_segment *out = (atsc_soft_data_segment *) output_items[0];

      int output_produced = 0;

      for (int i = 0; i < noutput_items; i++) {
        // check for a hit on the PN 511 pattern
        int errors = 0;

        for (int j = 0; j < LENGTH_511 && errors < PN511_ERROR_LIMIT; j++)
          errors += (in[i].data[j + OFFSET_511] >= 0) ^ atsc_pn511[j];

        //std::cout << errors << std::endl;

        if (errors < PN511_ERROR_LIMIT) { // 511 pattern is good.
          // determine if this is field 1 or field 2
          errors = 0;
          for (int j = 0; j < LENGTH_2ND_63; j++)
            errors += (in[i].data[j + OFFSET_2ND_63] >= 0) ^ atsc_pn63[j];

          // we should have either field 1 (== PN63) or field 2 (== ~PN63)
          if (errors <= PN63_ERROR_LIMIT) {
            //std::cout << "Found FIELD_SYNC_1" << std::endl;
            d_field_num = 1; // We are in field number 1 now
            d_segment_num = -1; // This is the first segment
          }
          else if (errors >= (LENGTH_2ND_63 - PN63_ERROR_LIMIT)) {
            //std::cout << "Found FIELD_SYNC_2" << std::endl;
            d_field_num = 2; // We are in field number 2 now
            d_segment_num = -1; // This is the first segment
          }
          else {
            // should be extremely rare.
            std::cerr << "!!! atsc_fs_checker: PN63 error count = " << errors << std::endl;
          }
        }

        if( d_field_num == 1 || d_field_num == 2 ) { // If we have sync
          // So we copy out current packet data to an output packet and fill its plinfo
          for( int j = 0; j < ATSC_DATA_SEGMENT_LENGTH; j++ )
            out[output_produced].data[j] = in[i].data[j];
          out[output_produced].pli.set_regular_seg((d_field_num == 2), d_segment_num);
          d_segment_num++;
          output_produced++;
        }
      }

      consume_each(noutput_items);
      return output_produced;
    }

  } /* namespace dtv */
} /* namespace gr */
