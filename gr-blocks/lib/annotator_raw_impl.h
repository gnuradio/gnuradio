/* -*- c++ -*- */
/*
 * Copyright 2010,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_ANNOTATOR_RAW_IMPL_H
#define	INCLUDED_GR_ANNOTATOR_RAW_IMPL_H

#include <gnuradio/blocks/annotator_raw.h>
#include <gnuradio/thread/thread.h>

namespace gr {
  namespace blocks {

    class annotator_raw_impl : public annotator_raw
    {
    private:
      size_t d_itemsize;
      std::vector<tag_t> d_queued_tags;
      gr::thread::mutex d_mutex;

    public:
      annotator_raw_impl(size_t sizeof_stream_item);
      ~annotator_raw_impl();

      // insert a tag to be added
      void add_tag(uint64_t offset, pmt::pmt_t key, pmt::pmt_t val);

      int work(int noutput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_ANNOTATOR_RAW_IMPL_H */
