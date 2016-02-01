/* -*- c++ -*- */
/*
 * Copyright 2014-2015 Free Software Foundation, Inc.
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

#include <multiply_const_vcc_impl.h>
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

namespace gr {
  namespace blocks {

    multiply_const_vcc::sptr
    multiply_const_vcc::make(std::vector<gr_complex> k)
    {
      return gnuradio::get_initial_sptr
        (new multiply_const_vcc_impl(k));
    }

    multiply_const_vcc_impl::multiply_const_vcc_impl(std::vector<gr_complex> k)
      : sync_block("multiply_const_vcc",
                   io_signature::make(1, 1, sizeof(gr_complex)*k.size()),
                   io_signature::make(1, 1, sizeof(gr_complex)*k.size())),
        d_k(k)
    {
      const int alignment_multiple =
	volk_get_alignment() / sizeof(gr_complex);
      set_alignment(std::max(1,alignment_multiple));
    }

    int
    multiply_const_vcc_impl::work(int noutput_items,
                                  gr_vector_const_void_star &input_items,
                                  gr_vector_void_star &output_items)
    {
      gr_complex *iptr = (gr_complex*)input_items[0];
      gr_complex *optr = (gr_complex*)output_items[0];

      int nitems_per_block = output_signature()->sizeof_stream_item(0)/sizeof(gr_complex);

      for(int i = 0; i < noutput_items; i++) {
	for(int j = 0; j < nitems_per_block; j++) {
	  *optr++ = *iptr++ * d_k[j];
        }
      }

      return noutput_items;
    }

    void
    multiply_const_vcc_impl::setup_rpc()
    {
#ifdef GR_CTRLPORT
      add_rpc_variable(
        rpcbasic_sptr(new rpcbasic_register_get<multiply_const_vcc,
                      std::vector<gr_complex> >(
	  alias(), "coefficient",
	  &multiply_const_vcc::k,
	  pmt::from_complex(-1024.0f),
          pmt::from_complex(1024.0f),
          pmt::from_complex(0.0f),
	  "", "Coefficient", RPC_PRIVLVL_MIN,
          DISPTIME | DISPOPTSTRIP)));

      add_rpc_variable(
        rpcbasic_sptr(new rpcbasic_register_set<multiply_const_vcc,
                      std::vector<gr_complex> >(
	  alias(), "coefficient",
	  &multiply_const_vcc::set_k,
	  pmt::from_complex(-1024.0f),
          pmt::from_complex(1024.0f),
          pmt::from_complex(0.0f),
	  "", "Coefficient",
	  RPC_PRIVLVL_MIN, DISPNULL)));
#endif /* GR_CTRLPORT */
    }

  } /* namespace blocks */
} /* namespace gr */
