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

#ifndef INCLUDED_DTV_ATSC_RS_DECODER_IMPL_H
#define INCLUDED_DTV_ATSC_RS_DECODER_IMPL_H

#include <gnuradio/dtv/atsc_rs_decoder.h>
#include "atsc_types.h"

extern "C" {
#include <gnuradio/fec/rs.h>
}

namespace gr {
  namespace dtv {

    class atsc_rs_decoder_impl : public atsc_rs_decoder
    {
    private:
      int nerrors_corrrected_count;
      int bad_packet_count;
      int total_packets;
      void *d_rs;

    public:
      atsc_rs_decoder_impl();
      ~atsc_rs_decoder_impl();

      /*!
       * Decode RS encoded packet.
       * \returns a count of corrected symbols, or -1 if the block was uncorrectible.
       */
      int decode(atsc_mpeg_packet_no_sync &out, const atsc_mpeg_packet_rs_encoded &in);

      int work(int noutput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items);
    };

  } /* namespace dtv */
} /* namespace gr */

#endif /* INCLUDED_DTV_ATSC_RS_DECODER_IMPL_H */
