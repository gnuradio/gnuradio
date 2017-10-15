/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_BLOCKS_FILE_SOURCE_IMPL_H
#define INCLUDED_BLOCKS_FILE_SOURCE_IMPL_H

#include <gnuradio/blocks/file_source.h>
#include <boost/thread/mutex.hpp>

namespace gr {
  namespace blocks {

    class BLOCKS_API file_source_impl : public file_source
    {
    private:
      size_t d_itemsize;
      FILE *d_fp;
      FILE *d_new_fp;
      bool d_repeat;
      bool d_updated;
      bool d_file_begin;
      long d_repeat_cnt;
      pmt::pmt_t d_add_begin_tag;

      boost::mutex fp_mutex;
      pmt::pmt_t _id;

      void do_update();

    public:
      file_source_impl(size_t itemsize, const char *filename, bool repeat);
      ~file_source_impl();

      bool seek(long seek_point, int whence);
      void open(const char *filename, bool repeat);
      void close();

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);

      void set_begin_tag(pmt::pmt_t val);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_FILE_SOURCE_IMPL_H */
