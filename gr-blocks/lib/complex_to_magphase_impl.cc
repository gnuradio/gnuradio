/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
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

#include "complex_to_magphase_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>
#include <gnuradio/math.h>

namespace gr {
  namespace blocks {

    complex_to_magphase::sptr complex_to_magphase::make(size_t vlen)
    {
      return gnuradio::get_initial_sptr(new complex_to_magphase_impl(vlen));
    }

    complex_to_magphase_impl::complex_to_magphase_impl(size_t vlen)
      : sync_block("complex_to_magphase",
		      io_signature::make (1, 1, sizeof(gr_complex)*vlen),
		      io_signature::make (2, 2, sizeof(float)*vlen)),
	d_vlen(vlen)
    {
      const int alignment_multiple =
	volk_get_alignment() / sizeof(float);
      set_alignment(std::max(1,alignment_multiple));
    }

    int
    complex_to_magphase_impl::work(int noutput_items,
				   gr_vector_const_void_star &input_items,
				   gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      float *out0 = (float *) output_items[0];
      float* out1 = (float *) output_items[1];
      int noi = noutput_items * d_vlen;

      volk_32fc_magnitude_32f_u(out0, in, noi);

      // The fast_atan2f is faster than Volk
      for (int i = 0; i < noi; i++){
	//    out[i] = std::arg (in[i]);
	out1[i] = gr::fast_atan2f(in[i]);
      }

      return noutput_items;
    }

  } /* namespace blocks */
}/* namespace gr */
