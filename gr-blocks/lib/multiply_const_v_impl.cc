/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2010,2012,2018 Free Software Foundation, Inc.
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

#include "multiply_const_v_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

namespace gr {
  namespace blocks {

    template <class T>
    typename multiply_const_v<T>::sptr multiply_const_v<T>::make(std::vector<T> k)
    {
      return gnuradio::get_initial_sptr(new multiply_const_v_impl<T>(k));
    }

    template <>
    multiply_const_v_impl<float>::multiply_const_v_impl(std::vector<float> k)
      : sync_block("multiply_const_vff",
                   io_signature::make(1, 1, sizeof(float)*k.size()),
                   io_signature::make(1, 1, sizeof(float)*k.size())),
        d_k(k)
    {
      const int alignment_multiple =
        volk_get_alignment() / sizeof(float);
      set_alignment(std::max(1,alignment_multiple));
    }

    template <>
    int
    multiply_const_v_impl<float>::work(int noutput_items,
                                  gr_vector_const_void_star &input_items,
                                  gr_vector_void_star &output_items)
    {
      float *iptr = (float*)input_items[0];
      float *optr = (float*)output_items[0];

      int nitems_per_block = this->output_signature()->sizeof_stream_item(0)/sizeof(float);

      for(int i = 0; i < noutput_items; i++) {
        for(int j = 0; j < nitems_per_block; j++) {
          *optr++ = *iptr++ * d_k[j];
        }
      }

      return noutput_items;
    }

    template <>
    multiply_const_v_impl<gr_complex>::multiply_const_v_impl(std::vector<gr_complex> k)
      : sync_block("multiply_const_vcc",
                   io_signature::make(1, 1, sizeof(gr_complex)*k.size()),
                   io_signature::make(1, 1, sizeof(gr_complex)*k.size())),
        d_k(k)
    {
      const int alignment_multiple =
	volk_get_alignment() / sizeof(gr_complex);
      set_alignment(std::max(1,alignment_multiple));
    }

    template <>
    int
    multiply_const_v_impl<gr_complex>::work(int noutput_items,
                                  gr_vector_const_void_star &input_items,
                                  gr_vector_void_star &output_items)
    {
      gr_complex *iptr = (gr_complex*)input_items[0];
      gr_complex *optr = (gr_complex*)output_items[0];

      int nitems_per_block = this->output_signature()->sizeof_stream_item(0)/sizeof(gr_complex);

      for(int i = 0; i < noutput_items; i++) {
	for(int j = 0; j < nitems_per_block; j++) {
	  *optr++ = *iptr++ * d_k[j];
        }
      }

      return noutput_items;
    }



    template <class T>
    multiply_const_v_impl<T>::multiply_const_v_impl(std::vector<T> k)
      : sync_block ("multiply_const_v<T>",
		       io_signature::make (1, 1, sizeof (T)*k.size()),
		       io_signature::make (1, 1, sizeof (T)*k.size())),
      d_k(k)
    {
    }

    template <class T>
    int
    multiply_const_v_impl<T>::work(int noutput_items,
		      gr_vector_const_void_star &input_items,
		      gr_vector_void_star &output_items)
    {
      T *iptr = (T *)input_items[0];
      T *optr = (T *)output_items[0];

      int nitems_per_block = this->output_signature()->sizeof_stream_item(0)/sizeof(T);

      for (int i = 0; i < noutput_items; i++)
	for (int j = 0; j < nitems_per_block; j++)
	  *optr++ = *iptr++ * d_k[j];

      return noutput_items;
    }

    template class multiply_const_v<std::int16_t>;
    template class multiply_const_v<std::int32_t>;
    template class multiply_const_v<float>;
    template class multiply_const_v<gr_complex>;
  } /* namespace blocks */
} /* namespace gr */
