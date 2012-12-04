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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "nop_impl.h"
#include <gr_io_signature.h>

namespace gr {
  namespace ctrlport {

    nop::sptr
    nop::make(size_t itemsize, int a, int b)
    {
      return gnuradio::get_initial_sptr
	(new nop_impl(itemsize, a, b));
    }


    nop_impl::nop_impl(size_t itemsize, int a, int b)
      : gr_sync_block("nop",
		      gr_make_io_signature(1, 1, itemsize),
		      gr_make_io_signature(0, 0, 0))
    {
      set_a(a);
      set_b(b);
      setup_rpc();
    }

    nop_impl::~nop_impl()
    {
    }

    void
    nop_impl::set_a(int a)
    {
      d_a = a;
    }

    void
    nop_impl::set_b(int b)
    {
      d_b = b;
    }

    int
    nop_impl::a() const
    {
      return d_a;
    }

    int
    nop_impl::b() const
    {
      return d_b;
    }

    int
    nop_impl::work(int noutput_items,
		   gr_vector_const_void_star &input_items,
		   gr_vector_void_star &output_items)
    {
      return noutput_items;
    }

    void
    nop_impl::setup_rpc()
    {
      d_get_32i_rpcs.push_back(get_32i_sptr
	 (new get_32i_t(d_name, "a", this, unique_id(),
			&nop_impl::a,
			pmt::mp(-128), pmt::mp(127), pmt::mp(0),
			"", "Value of a",
			RPC_PRIVLVL_MIN, DISPTIMESERIESF)));

      d_get_32i_rpcs.push_back(get_32i_sptr
	 (new get_32i_t(d_name, "b", this, unique_id(),
			&nop_impl::b,
			pmt::mp(-128), pmt::mp(127), pmt::mp(0),
			"", "Value of b",
			RPC_PRIVLVL_MIN, DISPTIMESERIESF)));

      d_set_32i_rpcs.push_back(set_32i_sptr
	 (new set_32i_t(d_name, "a", this, unique_id(),
			&nop_impl::set_a,
			pmt::mp(-128), pmt::mp(127), pmt::mp(0),
			"", "Value of a",
			RPC_PRIVLVL_MIN, DISPNULL)));

      d_set_32i_rpcs.push_back(set_32i_sptr
	 (new set_32i_t(d_name, "b", this, unique_id(),
			&nop_impl::set_b,
			pmt::mp(-128), pmt::mp(127), pmt::mp(0),
			"", "Value of b",
			RPC_PRIVLVL_MIN, DISPNULL)));
    }

  } /* namespace ctrlport */
} /* namespace gr */
