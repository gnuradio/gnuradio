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

#ifndef INCLUDED_GR_FILE_META_SINK_H
#define INCLUDED_GR_FILE_META_SINK_H

#include <gr_core_api.h>
#include <gr_sync_block.h>
#include <gr_file_sink_base.h>
#include <gruel/pmt.h>

using namespace pmt;

enum gr_file_types {
  GR_FILE_BYTE=0,
  GR_FILE_CHAR=0,
  GR_FILE_SHORT=1,
  GR_FILE_INT,
  GR_FILE_LONG,
  GR_FILE_LONG_LONG,
  GR_FILE_FLOAT,
  GR_FILE_DOUBLE,
};

class gr_file_meta_sink;
typedef boost::shared_ptr<gr_file_meta_sink> gr_file_meta_sink_sptr;

GR_CORE_API gr_file_meta_sink_sptr
gr_make_file_meta_sink(size_t itemsize, const char *filename,
		       double samp_rate, gr_file_types type, bool complex,
		       const std::string &extra_dict="");

/*!
 * \brief Write stream to file.
 * \ingroup sink_blk
 */

class GR_CORE_API gr_file_meta_sink : public gr_sync_block, public gr_file_sink_base
{
  
  friend GR_CORE_API gr_file_meta_sink_sptr
    gr_make_file_meta_sink(size_t itemsize, const char *filename,
			   double samp_rate, gr_file_types type, bool complex,
			   const std::string &extra_dict);

 private:
  size_t d_itemsize;
  pmt_t d_header;
  pmt_t d_extra_dict;

 protected:
  gr_file_meta_sink(size_t itemsize, const char *filename,
		    double samp_rate, gr_file_types type, bool complex,
		    const std::string &extra_dict);


  void write_header(pmt_t header);

 public:
  ~gr_file_meta_sink();

  int work(int noutput_items,
	   gr_vector_const_void_star &input_items,
	   gr_vector_void_star &output_items);
};

#endif /* INCLUDED_GR_FILE_META_SINK_H */
