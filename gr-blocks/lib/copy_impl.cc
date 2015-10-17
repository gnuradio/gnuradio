/* -*- c++ -*- */
/*
 * Copyright 2006,2009,2013 Free Software Foundation, Inc.
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

#include "copy_impl.h"
#include <gnuradio/io_signature.h>
#include <string.h>

namespace gr {
  namespace blocks {

    copy::sptr
    copy::make(size_t itemsize)
    {
      return gnuradio::get_initial_sptr
        (new copy_impl(itemsize));
    }

    copy_impl::copy_impl(size_t itemsize)
      : block("copy",
                 io_signature::make(1, -1, itemsize),
                 io_signature::make(1, -1, itemsize)),
        d_itemsize(itemsize),
        d_enabled(true)
    {
      message_port_register_in(pmt::mp("en"));
      set_msg_handler(pmt::mp("en"),
                      boost::bind(&copy_impl::handle_enable, this, _1));
    }

    copy_impl::~copy_impl()
    {
    }

    void
    copy_impl::handle_enable(pmt::pmt_t msg)
    {
      if(pmt::is_bool(msg)) {
        bool en = pmt::to_bool(msg);
        d_enabled = en;
      }
    }

    void
    copy_impl::forecast(int noutput_items, gr_vector_int &ninput_items_required)
    {
      unsigned ninputs = ninput_items_required.size();
      for (unsigned i = 0; i < ninputs; i++)
	ninput_items_required[i] = noutput_items;
    }

    bool
    copy_impl::check_topology(int ninputs, int noutputs)
    {
      return ninputs == noutputs;
    }

    int
    copy_impl::general_work(int noutput_items,
                            gr_vector_int &ninput_items,
                            gr_vector_const_void_star &input_items,
                            gr_vector_void_star &output_items)
    {
      const uint8_t **in = (const uint8_t**)&input_items[0];
      uint8_t **out = (uint8_t**)&output_items[0];

      int n = 0;
      if(d_enabled) {
        int ninputs = input_items.size();
        for(int i = 0; i < ninputs; i++) {
          memcpy(out[i], in[i], noutput_items*d_itemsize);
        }
        n = noutput_items;
      }

      consume_each(noutput_items);
      return n;
    }


    void
    copy_impl::setup_rpc()
    {
#ifdef GR_CTRLPORT
      add_rpc_variable(
        rpcbasic_sptr(new rpcbasic_register_handler<copy>(
	  alias(), "en",
	  "", "Enable",
	  RPC_PRIVLVL_MIN, DISPNULL)));
#endif /* GR_CTRLPORT */
    }

  } /* namespace blocks */
} /* namespace gr */
