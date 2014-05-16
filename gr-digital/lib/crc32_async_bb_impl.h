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

#ifndef INCLUDED_DIGITAL_CRC32_ASYNC_BB_IMPL_H
#define INCLUDED_DIGITAL_CRC32_ASYNC_BB_IMPL_H

#include <gnuradio/digital/crc32_async_bb.h>
#include <boost/crc.hpp>

namespace gr {
  namespace digital {

    class crc32_async_bb_impl : public crc32_async_bb
    {
     private:
      bool d_check;
      boost::crc_optimal<32, 0x04C11DB7, 0xFFFFFFFF, 0xFFFFFFFF, true, true> d_crc_impl;

      pmt::pmt_t d_in_port;
      pmt::pmt_t d_out_port;

      void calc(pmt::pmt_t msg);
      void check(pmt::pmt_t msg);

     public:
      crc32_async_bb_impl(bool check);
      ~crc32_async_bb_impl();

      int general_work(int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items);

      uint64_t d_npass;
      uint64_t d_nfail;
    };

  } // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_CRC32_ASYNC_BB_IMPL_H */
