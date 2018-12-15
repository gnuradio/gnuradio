/* -*- c++ -*- */
/*
 * Copyright 2012,2018 Free Software Foundation, Inc.
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

#include "and_const_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
  namespace blocks {

    template <class T>
    typename and_const<T>::sptr and_const<T>::make(T k)
    {
      return gnuradio::get_initial_sptr(new and_const_impl<T> (k));
    }

    template <class T>
    and_const_impl<T> ::and_const_impl (T k)
      : sync_block ("and_const",
		       io_signature::make (1, 1, sizeof (T)),
		       io_signature::make (1, 1, sizeof (T))),
      d_k(k)
    {
    }

    template <class T>
    int
    and_const_impl<T> ::work(int noutput_items,
		      gr_vector_const_void_star &input_items,
		      gr_vector_void_star &output_items)
    {
      T *iptr = (T *) input_items[0];
      T *optr = (T *) output_items[0];

      int size = noutput_items;

      while (size >= 8){
	*optr++ = *iptr++ & d_k;
	*optr++ = *iptr++ & d_k;
	*optr++ = *iptr++ & d_k;
	*optr++ = *iptr++ & d_k;
	*optr++ = *iptr++ & d_k;
	*optr++ = *iptr++ & d_k;
	*optr++ = *iptr++ & d_k;
	*optr++ = *iptr++ & d_k;
	size -= 8;
      }

      while (size-- > 0)
	*optr++ = *iptr++ & d_k;

      return noutput_items;
    }

template class and_const<std::uint8_t>;
template class and_const<std::int16_t>;
template class and_const<std::int32_t>;
  } /* namespace blocks */
} /* namespace gr */
