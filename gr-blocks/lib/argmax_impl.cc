/* -*- c++ -*- */
/*
 * Copyright 2007,2013,2018 Free Software Foundation, Inc.
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

#include "argmax_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
  namespace blocks {

    template <class T>
    typename argmax<T>::sptr
    argmax<T>::make(size_t vlen)
    {
      return gnuradio::get_initial_sptr
        (new argmax_impl<T>(vlen));
    }

    template <class T>
    argmax_impl<T>::argmax_impl(size_t vlen)
    : sync_block("argmax",
                    io_signature::make(1, -1, vlen*sizeof(T)),
                    io_signature::make(2, 2, sizeof(std::int16_t))),
      d_vlen(vlen)
    {
    }

    template <class T>
    argmax_impl<T>::~argmax_impl()
    {
    }

    template <class T>
    int
    argmax_impl<T>::work(int noutput_items,
                      gr_vector_const_void_star &input_items,
                      gr_vector_void_star &output_items)
    {
      int ninputs = input_items.size ();

      std::int16_t *x_optr = (std::int16_t *)output_items[0];
      std::int16_t *y_optr = (std::int16_t *)output_items[1];

      for(int i = 0; i < noutput_items; i++) {
        T max = ((T *)input_items[0])[i*d_vlen];
        int x = 0;
        int y = 0;

        for(int j = 0; j < (int)d_vlen; j++ ) {
          for(int k = 0; k < ninputs; k++) {
            if(((T *)input_items[k])[i*d_vlen + j] > max) {
              max = ((T *)input_items[k])[i*d_vlen + j];
              x = j;
              y = k;
            }
          }
        }

        *x_optr++ = (std::int16_t)x;
        *y_optr++ = (std::int16_t)y;
      }
      return noutput_items;
    }
    template class argmax<float>;
    template class argmax<std::int32_t>;
    template class argmax<std::int16_t>;

  } /* namespace blocks */
} /* namespace gr */
