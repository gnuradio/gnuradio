/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2012 Free Software Foundation, Inc.
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

/* @WARNING@ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "@IMPL_NAME@.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

namespace gr {
  namespace filter {

    @BASE_NAME@::sptr
    @BASE_NAME@::make(int decimation, const std::vector<@TAP_TYPE@> &taps)
    {
      return gnuradio::get_initial_sptr(new @IMPL_NAME@
					(decimation, taps));
    }


    @IMPL_NAME@::@IMPL_NAME@(int decimation, const std::vector<@TAP_TYPE@> &taps)
      : sync_decimator("@BASE_NAME@",
			  io_signature::make(1, 1, sizeof(@I_TYPE@)),
			  io_signature::make(1, 1, sizeof(@O_TYPE@)),
			  decimation)
    {
      d_fir = new kernel::@BASE_NAME@(decimation, taps);
      d_updated = false;
      set_history(d_fir->ntaps());

      const int alignment_multiple =
	volk_get_alignment() / sizeof(float);
      set_alignment(std::max(1, alignment_multiple));
    }

    @IMPL_NAME@::~@IMPL_NAME@()
    {
      delete d_fir;
    }

    void
    @IMPL_NAME@::set_taps(const std::vector<@TAP_TYPE@> &taps)
    {
      gr::thread::scoped_lock l(d_setlock);
      d_fir->set_taps(taps);
      d_updated = true;
    }

    std::vector<@TAP_TYPE@>
    @IMPL_NAME@::taps() const
    {
      return d_fir->taps();
    }

    int
    @IMPL_NAME@::work(int noutput_items,
		      gr_vector_const_void_star &input_items,
		      gr_vector_void_star &output_items)
    {
      gr::thread::scoped_lock l(d_setlock);

      const @I_TYPE@ *in = (const @I_TYPE@*)input_items[0];
      @O_TYPE@ *out = (@O_TYPE@*)output_items[0];

      if (d_updated) {
	set_history(d_fir->ntaps());
	d_updated = false;
	return 0;	     // history requirements may have changed.
      }

      if (decimation() == 1) {
	d_fir->filterN(out, in, noutput_items);
      }
      else {
	d_fir->filterNdec(out, in, noutput_items,
			  decimation());
      }

      return noutput_items;
    }

  } /* namespace filter */
} /* namespace gr */

