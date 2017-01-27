/* -*- c++ -*- */
/* Copyright 2016 Free Software Foundation, Inc.
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

#include <iostream>
#include <iomanip>
#include <string.h>
#include <volk/volk.h>
#include <gnuradio/digital/header_format_counter.h>
#include <gnuradio/digital/header_buffer.h>
#include <gnuradio/math.h>

namespace gr {
  namespace digital {

    header_format_counter::sptr
    header_format_counter::make(const std::string &access_code,
                                int threshold, int bps)
    {
      return header_format_counter::sptr
        (new header_format_counter(access_code, threshold, bps));
    }

    header_format_counter::header_format_counter(const std::string &access_code,
                                                 int threshold, int bps)
      : header_format_default(access_code, threshold, bps)
    {
      d_counter = 0;
    }

    header_format_counter::~header_format_counter()
    {
    }

    bool
    header_format_counter::format(int nbytes_in,
                                  const unsigned char *input,
                                  pmt::pmt_t &output,
                                  pmt::pmt_t &info)

    {
      uint8_t* bytes_out = (uint8_t*)volk_malloc(header_nbytes(),
                                                 volk_get_alignment());

      header_buffer header(bytes_out);
      header.add_field64(d_access_code, d_access_code_len);
      header.add_field16((uint16_t)(nbytes_in));
      header.add_field16((uint16_t)(nbytes_in));
      header.add_field16((uint16_t)(d_bps));
      header.add_field16((uint16_t)(d_counter));

      // Package output data into a PMT vector
      output = pmt::init_u8vector(header_nbytes(), bytes_out);

      // Creating the output pmt copies data; free our own here.
      volk_free(bytes_out);

      d_counter++;

      return true;
    }

    size_t
    header_format_counter::header_nbits() const
    {
      return d_access_code_len + 8*4*sizeof(uint16_t);
    }

    bool
    header_format_counter::header_ok()
    {
      // confirm that two copies of header info are identical
      uint16_t len0 = d_hdr_reg.extract_field16(0);
      uint16_t len1 = d_hdr_reg.extract_field16(16);
      return (len0 ^ len1) == 0;
    }

    int
    header_format_counter::header_payload()
    {
      uint16_t len = d_hdr_reg.extract_field16(0);
      uint16_t bps = d_hdr_reg.extract_field16(32);
      uint16_t counter = d_hdr_reg.extract_field16(48);

      d_bps = bps;

      d_info = pmt::make_dict();
      d_info = pmt::dict_add(d_info, pmt::intern("payload symbols"),
                             pmt::from_long(8*len / d_bps));
      d_info = pmt::dict_add(d_info, pmt::intern("bps"),
                             pmt::from_long(bps));
      d_info = pmt::dict_add(d_info, pmt::intern("counter"),
                             pmt::from_long(counter));
      return static_cast<int>(len);
    }

  } /* namespace digital */
} /* namespace gr */
