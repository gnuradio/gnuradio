/* -*- c++ -*- */
/*
 * Copyright 2012,2013,2018 Free Software Foundation, Inc.
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


#ifndef VECTOR_INSERT_IMPL_H
#define VECTOR_INSERT_IMPL_H

#include <gnuradio/blocks/vector_insert.h>

namespace gr {
  namespace blocks {

template<class T>
    class vector_insert_impl : public  vector_insert<T>
    {
    private:
      std::vector<T> d_data;
      int d_offset;
      int d_periodicity;

    public:
      vector_insert_impl (const std::vector<T> &data,
                  int periodicity, int offset);
      ~vector_insert_impl ();

      void rewind() { d_offset=0; }
      void set_data(const std::vector<T> &data) {
        d_data = data; rewind(); }

      int general_work(int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* VECTOR_INSERT_IMPL_H */
