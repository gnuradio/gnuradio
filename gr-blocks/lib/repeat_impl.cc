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

#include "repeat_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
  namespace blocks {

    repeat::sptr repeat::make(size_t itemsize, int interp)
    {
      return gnuradio::get_initial_sptr(new repeat_impl(itemsize, interp));
    }

    repeat_impl::repeat_impl(size_t itemsize, int interp)
      : sync_interpolator("repeat",
			     io_signature::make (1, 1, itemsize),
			     io_signature::make (1, 1, itemsize),
			     interp),
	d_itemsize(itemsize),
	d_interp(interp)
    {
        message_port_register_in(pmt::mp("interpolation"));
        set_msg_handler(pmt::mp("interpolation"),
                boost::bind(&repeat_impl::msg_set_interpolation, this, _1));
    }

    void
    repeat_impl::msg_set_interpolation(pmt::pmt_t msg)
    {
        // Dynamization by Kevin McQuiggin:
        d_interp = pmt::to_long(pmt::cdr(msg));
        sync_interpolator::set_interpolation(d_interp);
    }
    void
    repeat_impl::set_interpolation(int interp)
    {
        // This ensures that interpolation is only changed between calls to work
        // (and not in the middle of an ongoing work)
        _post(  pmt::mp("interpolation"), /* port */
                pmt::cons(pmt::mp("interpolation"), pmt::from_long(interp)) /* pair */
        );
    }

    int
    repeat_impl::work(int noutput_items,
		      gr_vector_const_void_star &input_items,
		      gr_vector_void_star &output_items)
    {
      const char *in = (const char *) input_items[0];
      char *out = (char *)output_items[0];

      for (int i = 0; i < noutput_items/d_interp; i++) {
	for (int j = 0; j < d_interp; j++) {
	  memcpy(out, in, d_itemsize);
	  out += d_itemsize;
	}

	in += d_itemsize;
      }

      return noutput_items;
    }

  } /* namespace blocks */
} /* namespace gr */
