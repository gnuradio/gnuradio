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

#ifndef INCLUDED_BLOCKS_FILE_META_SOURCE_IMPL_H
#define INCLUDED_BLOCKS_FILE_META_SOURCE_IMPL_H

#include <gnuradio/blocks/file_meta_source.h>
#include <gnuradio/tags.h>
#include <pmt/pmt.h>
#include <gnuradio/thread/thread.h>

#include <gnuradio/blocks/file_meta_sink.h>

using namespace pmt;

namespace gr {
  namespace blocks {

    class file_meta_source_impl : public file_meta_source
    {
    private:
      enum meta_state_t {
	STATE_INLINE=0,
	STATE_DETACHED
      };

      size_t d_itemsize;
      double d_samp_rate;
      pmt_t d_time_stamp;
      size_t d_seg_size;
      bool d_updated;
      bool d_repeat;

      FILE *d_new_fp, *d_new_hdr_fp;
      FILE *d_fp, *d_hdr_fp;
      meta_state_t d_state;

      std::vector<tag_t> d_tags;

    protected:
      bool _open(FILE **fp, const char *filename);
      bool read_header(pmt_t &hdr, pmt_t &extras);
      void parse_header(pmt_t hdr, uint64_t offset,
			std::vector<tag_t> &tags);
      void parse_extras(pmt_t extras, uint64_t offset,
			std::vector<tag_t> &tags);

    public:
      file_meta_source_impl(const std::string &filename,
			    bool repeat=false,
			    bool detached_header=false,
			    const std::string &hdr_filename="");

      ~file_meta_source_impl();

      bool open(const std::string &filename,
		const std::string &hdr_filename="");
      void close();
      void do_update();

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_FILE_META_SOURCE_IMPL_H */
