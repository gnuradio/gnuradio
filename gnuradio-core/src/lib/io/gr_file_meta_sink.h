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
		       double samp_rate=1, double relative_rate=1,
		       gr_file_types type=GR_FILE_FLOAT, bool complex=true,
		       const std::string &extra_dict="");

/*!
 * \brief Write stream to file with meta-data headers.
 * \ingroup sink_blk
 *
 * These files represent data as binary information in between
 * meta-data headers. The headers contain information about the type
 * of data and properties of the data in the next segment of
 * samples. The information includes:
 *
 *   rx_rate (double): sample rate of data.
 *   rx_time (uint64_t, double): time stamp of first sample in segment.
 *   type (gr_file_types as int32_t): data type.
 *   cplx (bool): Is data complex?
 *   strt (uint64_t): Starting byte of data in this segment.
 *   size (uint64_t): Size in bytes of data in this segment.
 *
 * Tags can be sent to the file to update the information, which will
 * create a new header. Headers are found by searching from the first
 * header (at position 0 in the file) and reading where the data
 * segment starts plus the data segment size. Following will either be
 * a new header or EOF.
 */
class GR_CORE_API gr_file_meta_sink : public gr_sync_block, public gr_file_sink_base
{
  /*!
   * \brief Create a meta-data file sink.
   *
   * \param itemsize (size_t): Size of data type.
   * \param filename (string): Name of file to write data to.
   * \param samp_rate (double): Sample rate of data. If sample rate will be
   *    set by a tag, such as rx_tag from a UHD source, this is
   *    basically ignored.
   * \param relative_rate (double): Rate chance from source of sample
   *    rate tag to sink.
   * \param type (gr_file_types): Data type (int, float, etc.)
   * \param complex (bool): If data stream is complex
   * \param extra_dict (string): a serialized PMT dictionary of extra
   *    information. Currently not supported.
   */
  friend GR_CORE_API gr_file_meta_sink_sptr
    gr_make_file_meta_sink(size_t itemsize, const char *filename,
			   double samp_rate, double relative_rate,
			   gr_file_types type, bool complex,
			   const std::string &extra_dict);

 private:
  size_t d_itemsize;
  double d_relative_rate;
  uint64_t d_total_seg_size;
  pmt_t d_header;
  pmt_t d_extra;
  size_t d_extra_size;

 protected:
  gr_file_meta_sink(size_t itemsize, const char *filename,
		    double samp_rate=1, double relative_rate=1,
		    gr_file_types type=GR_FILE_FLOAT, bool complex=true,
		    const std::string &extra_dict="");

  void write_header(pmt_t header, pmt_t extra);
  bool update_header(pmt_t key, pmt_t value);
  uint64_t get_last_header_loc();

 public:
  ~gr_file_meta_sink();

  int work(int noutput_items,
	   gr_vector_const_void_star &input_items,
	   gr_vector_void_star &output_items);
};

#endif /* INCLUDED_GR_FILE_META_SINK_H */
