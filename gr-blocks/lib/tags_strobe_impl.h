/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_TAGS_STROBE_IMPL_H
#define INCLUDED_GR_TAGS_STROBE_IMPL_H

#include <gnuradio/blocks/tags_strobe.h>

namespace gr {
  namespace blocks {

    class BLOCKS_API tags_strobe_impl : public tags_strobe
    {
    private:
      size_t d_itemsize;
      uint64_t d_nsamps;
      tag_t d_tag;
      uint64_t d_offset;

      void run();

    public:
      tags_strobe_impl(size_t sizeof_stream_item,
                       pmt::pmt_t value, uint64_t nsamps,
                       pmt::pmt_t key);
      ~tags_strobe_impl();

      void set_value(pmt::pmt_t value);
      void set_key(pmt::pmt_t key);
      pmt::pmt_t value() const { return d_tag.value; }
      pmt::pmt_t key() const { return d_tag.key; }
      void set_nsamps(uint64_t nsamps);
      uint64_t nsamps() const { return d_nsamps; }

      int work(int noutput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_TAGS_STROBE_IMPL_H */
