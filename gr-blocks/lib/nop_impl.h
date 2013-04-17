/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_NOP_IMPL_H
#define INCLUDED_GR_NOP_IMPL_H

#include <gnuradio/blocks/nop.h>

namespace gr {
  namespace blocks {

    class nop_impl : public nop
    {
    protected:
      int d_nmsgs_recvd;
      int d_ctrlport_test;

      // Method that just counts any received messages.
      void count_received_msgs(pmt::pmt_t msg);

    public:
      nop_impl(size_t sizeof_stream_item);
      ~nop_impl();

      void setup_rpc();

      int nmsgs_received() const { return d_nmsgs_recvd; }

      int  ctrlport_test() const { return d_ctrlport_test; }
      void set_ctrlport_test(int x) { d_ctrlport_test = x; }

      int general_work(int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_NOP_IMPL_H */
