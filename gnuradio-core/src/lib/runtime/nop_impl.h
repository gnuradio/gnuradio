/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_CTRLPORT_NOP_IMPL_H
#define INCLUDED_CTRLPORT_NOP_IMPL_H

#include <ctrlport/nop.h>
#include <ctrlport/rpcregisterhelpers.h>
#include <boost/thread/shared_mutex.hpp>

namespace gr {
  namespace ctrlport {

    class CTRLPORT_API nop_impl : public nop
    {
    private:
      typedef rpcbasic_register_get<nop_impl, int> get_32i_t;
      typedef rpcbasic_register_set<nop_impl, int> set_32i_t;

      typedef boost::shared_ptr<get_32i_t> get_32i_sptr;
      typedef boost::shared_ptr<set_32i_t> set_32i_sptr;

      std::vector<get_32i_sptr> d_get_32i_rpcs;
      std::vector<set_32i_sptr> d_set_32i_rpcs;

      void setup_rpc();

      int d_a, d_b;

    public:
      nop_impl(size_t itemsize, int a, int b);
      ~nop_impl();

      void set_a(int a);
      void set_b(int b);
      int a() const;
      int b() const;

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace ctrlport */
} /* namespace gr */

#endif /* INCLUDED_CTRLPORT_NOP_IMPL_H */

