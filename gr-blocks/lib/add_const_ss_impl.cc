/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2010,2012,2015 Free Software Foundation, Inc.
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

#include <add_const_ss_impl.h>
#include <gnuradio/io_signature.h>

namespace gr {
  namespace blocks {

    add_const_ss::sptr add_const_ss::make(short k)
    {
      return gnuradio::get_initial_sptr
        (new add_const_ss_impl(k));
    }

    add_const_ss_impl::add_const_ss_impl(short k)
      : sync_block("add_const_ss",
                   io_signature::make (1, 1, sizeof(short)),
                   io_signature::make (1, 1, sizeof(short))),
        d_k(k)
    {
    }

    add_const_ss_impl::~add_const_ss_impl()
    {
    }

    int
    add_const_ss_impl::work(int noutput_items,
                            gr_vector_const_void_star &input_items,
                            gr_vector_void_star &output_items)
    {
      const short *iptr = (const short *) input_items[0];
      short *optr = (short *) output_items[0];

      int size = noutput_items;

      while(size >= 8) {
	*optr++ = *iptr++ + d_k;
	*optr++ = *iptr++ + d_k;
	*optr++ = *iptr++ + d_k;
	*optr++ = *iptr++ + d_k;
	*optr++ = *iptr++ + d_k;
	*optr++ = *iptr++ + d_k;
	*optr++ = *iptr++ + d_k;
	*optr++ = *iptr++ + d_k;
	size -= 8;
      }

      while(size-- > 0) {
	*optr++ = *iptr++ + d_k;
      }

      return noutput_items;
    }

    void
    add_const_ss_impl::setup_rpc()
    {
#ifdef GR_CTRLPORT
      add_rpc_variable(
        rpcbasic_sptr(new rpcbasic_register_get<add_const_ss, short>(
	  alias(), "Constant",
	  &add_const_ss::k,
	  pmt::from_long(-65536),
          pmt::from_long(65535),
          pmt::from_long(0),
	  "", "Constant to add", RPC_PRIVLVL_MIN,
          DISPTIME | DISPOPTCPLX | DISPOPTSTRIP)));

      add_rpc_variable(
        rpcbasic_sptr(new rpcbasic_register_set<add_const_ss, short>(
	  alias(), "Constant",
	  &add_const_ss::set_k,
	  pmt::from_long(-65536),
          pmt::from_long(65535),
          pmt::from_long(0),
	  "", "Constant to add",
	  RPC_PRIVLVL_MIN, DISPNULL)));
#endif /* GR_CTRLPORT */
    }

  } /* namespace blocks */
} /* namespace gr */
