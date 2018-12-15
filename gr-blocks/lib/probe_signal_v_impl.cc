/* -*- c++ -*- */
/*
 * Copyright 2005,2010,2012-2013,2018 Free Software Foundation, Inc.
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


#include "probe_signal_v_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
  namespace blocks {

    template <class T>
    typename probe_signal_v<T>::sptr
    probe_signal_v<T>::make(size_t size)
    {
      return gnuradio::get_initial_sptr
        (new probe_signal_v_impl<T>(size));
    }

    template <class T>
    probe_signal_v_impl<T>::probe_signal_v_impl(size_t size)
    : sync_block("probe_signal_v",
                    io_signature::make(1, 1, size*sizeof(T)),
                    io_signature::make(0, 0, 0)),
      d_level(size, 0), d_size(size)
    {
    }

    template <class T>
    probe_signal_v_impl<T>::~probe_signal_v_impl()
    {
    }

    template <class T>
    int
    probe_signal_v_impl<T>::work(int noutput_items,
                      gr_vector_const_void_star &input_items,
                      gr_vector_void_star &output_items)
    {
      const T *in = (const T *)input_items[0];

      for(size_t i=0; i<d_size; i++)
	d_level[i] = in[(noutput_items-1)*d_size+i];

      return noutput_items;
    }
    template class probe_signal_v<std::uint8_t>;
    template class probe_signal_v<std::int16_t>;
    template class probe_signal_v<std::int32_t>;
    template class probe_signal_v<float>;
    template class probe_signal_v<gr_complex>;

  } /* namespace blocks */
} /* namespace gr */
