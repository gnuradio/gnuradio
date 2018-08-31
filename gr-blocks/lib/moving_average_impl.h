/* -*- c++ -*- */
/*
 * Copyright 2008,2013,2017-2018 Free Software Foundation, Inc.
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


#ifndef MOVING_AVERAGE_IMPL_H
#define MOVING_AVERAGE_IMPL_H

#include <gnuradio/blocks/moving_average.h>
#include <vector>
#include <algorithm>

namespace gr {
  namespace blocks {

template<class T>
    class moving_average_impl : public  moving_average<T>
    {
    private:
      int d_length;
      T d_scale;
      int d_max_iter;
      unsigned int d_vlen;
      std::vector<T> d_sum;


      int d_new_length;
      T d_new_scale;
      bool d_updated;

    public:
      moving_average_impl (int length, T scale,
                  int max_iter = 4096,
                  unsigned int vlen = 1);
      ~moving_average_impl ();

      int length() const { return d_new_length; }
      T scale() const { return d_new_scale; }
      unsigned int vlen() const { return d_vlen; }

      void set_length_and_scale(int length, T scale);
      void set_length(int length);
      void set_scale(T scale);

      int work(int noutput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items);

    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* MOVING_AVERAGE_IMPL_H */
