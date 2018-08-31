/* -*- c++ -*- */
/*
 * Copyright 2005,2012-2013,2018 Free Software Foundation, Inc.
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

#ifndef PROBE_SIGNAL_V_IMPL_H
#define PROBE_SIGNAL_V_IMPL_H

#include <vector>
#include <gnuradio/blocks/probe_signal_v.h>

namespace gr {
  namespace blocks {

    template <class T>
    class probe_signal_v_impl : public probe_signal_v<T>
    {
    private:
      std::vector<T> d_level;
      size_t d_size;

    public:
      probe_signal_v_impl(size_t size);
      ~probe_signal_v_impl();

      std::vector<T> level() const { return d_level; }

      int work(int noutput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* PROBE_SIGNAL_V_IMPL_H */
