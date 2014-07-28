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

#ifndef INCLUDED_BLOCKS_FILE_META_SINK_IMPL_H
#define INCLUDED_BLOCKS_FILE_META_SINK_IMPL_H

#include <gnuradio/blocks/file_meta_sink.h>
#include <pmt/pmt.h>

using namespace pmt;

namespace gr {
  namespace blocks {

    class file_meta_sink_impl : public file_meta_sink
    {
    private:
      enum meta_state_t {
	STATE_INLINE=0,
	STATE_DETACHED
      };

      size_t d_itemsize;
      double d_samp_rate;
      double d_relative_rate;
      size_t d_max_seg_size;
      size_t d_total_seg_size;
      pmt_t d_header;
      pmt_t d_extra;
      size_t d_extra_size;
      bool d_updated;
      bool d_unbuffered;

      FILE *d_new_fp, *d_new_hdr_fp;
      FILE *d_fp, *d_hdr_fp;
      meta_state_t d_state;

    protected:
      void write_header(FILE *fp, pmt_t header, pmt_t extra);
      void update_header(pmt_t key, pmt_t value);
      void update_last_header();
      void update_last_header_inline();
      void update_last_header_detached();
      void write_and_update();
      void update_rx_time();

      bool _open(FILE **fp, const char *filename);

    public:
      file_meta_sink_impl(size_t itemsize, const std::string &filename,
			  double samp_rate=1, double relative_rate=1,
			  gr_file_types type=GR_FILE_FLOAT, bool complex=true,
			  size_t max_segment_size=1000000,
			  const std::string &extra_dict="",
			  bool detached_header=false);
      ~file_meta_sink_impl();

      bool open(const std::string &filename);
      void close();
      void do_update();

      void set_unbuffered(bool unbuffered)
      {
	d_unbuffered = unbuffered;
      }

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_FILE_META_SINK_IMPL_H */
