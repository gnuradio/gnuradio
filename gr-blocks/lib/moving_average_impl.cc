/* -*- c++ -*- */
/*
 * Copyright 2008,2010,2013,2017,2018 Free Software Foundation, Inc.
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

#include "moving_average_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
  namespace blocks {

    template <class T>
    typename moving_average<T>::sptr
    moving_average<T>::make(int length, T scale, int max_iter, unsigned int vlen)
    {
      return gnuradio::get_initial_sptr
        (new moving_average_impl<T> (length, scale, max_iter, vlen));
    }

    template <class T>
    moving_average_impl<T>::moving_average_impl(int length, T scale, int max_iter, unsigned int vlen)
    : sync_block("moving_average",
                    io_signature::make(1, 1, sizeof(T)*vlen),
                    io_signature::make(1, 1, sizeof(T)*vlen)),
      d_length(length),
      d_scale(scale),
      d_max_iter(max_iter),
      d_vlen(vlen),
      d_new_length(length),
      d_new_scale(scale),
      d_updated(false)
    {
      this->set_history(length);
      //we don't have C++11's <array>, so initialize the stored vector instead
      //we store this vector so that work() doesn't spend its time allocating and freeing vector storage
      if(d_vlen > 1) {
        d_sum = std::vector<T>(d_vlen);
      }
    }

    template <class T>
    moving_average_impl<T> ::~moving_average_impl()
    {
    }

    template <class T>
    void
    moving_average_impl<T> ::set_length_and_scale(int length, T scale)
    {
      d_new_length = length;
      d_new_scale = scale;
      d_updated = true;
    }

    template <class T>
    void
    moving_average_impl<T> ::set_length(int length)
    {
      d_new_length = length;
      d_updated = true;
    }

    template <class T>
    void
    moving_average_impl<T> ::set_scale(T scale)
    {
      d_new_scale = scale;
      d_updated = true;
    }

    template <class T>
    int
    moving_average_impl<T> ::work(int noutput_items,
                      gr_vector_const_void_star &input_items,
                      gr_vector_void_star &output_items)
    {
      if(d_updated) {
        d_length = d_new_length;
        d_scale = d_new_scale;
        this->set_history(d_length);
        d_updated = false;
        return 0; // history requirements might have changed
      }

      const T *in = (const T *)input_items[0];
      T *out = (T *)output_items[0];

      unsigned int num_iter = (unsigned int)((noutput_items>d_max_iter) ? d_max_iter : noutput_items);
      if(d_vlen == 1) {
        T sum = in[0];
        for(int i = 1; i < d_length-1; i++) {
          sum += in[i];
        }

        for(unsigned int i = 0; i < num_iter; i++) {
          sum += in[i+d_length-1];
          out[i] = sum * d_scale;
          sum -= in[i];
        }

      } else { // d_vlen > 1
        //gets automatically optimized well
        for(unsigned int elem = 0; elem < d_vlen; elem++) {
          d_sum[elem] = in[elem];
        }

        for(int i = 1; i < d_length - 1; i++) {
          for(unsigned int elem = 0; elem < d_vlen; elem++) {
            d_sum[elem] += in[i*d_vlen + elem];
          }
        }

        for(unsigned int i = 0; i < num_iter; i++) {
          for(unsigned int elem = 0; elem < d_vlen; elem++) {
            d_sum[elem] += in[(i+d_length-1)*d_vlen + elem];
            out[i*d_vlen + elem] = d_sum[elem] * d_scale;
            d_sum[elem] -= in[i*d_vlen + elem];
          }
        }
      }
      return num_iter;
    }

template class moving_average<std::int16_t>;
template class moving_average<std::int32_t>;
template class moving_average<float>;
template class moving_average<gr_complex>;
  } /* namespace blocks */
} /* namespace gr */
