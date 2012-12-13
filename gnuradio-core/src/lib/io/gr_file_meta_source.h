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

#ifndef INCLUDED_GR_FILE_META_SOURCE_H
#define INCLUDED_GR_FILE_META_SOURCE_H

#include <gr_core_api.h>
#include <gr_sync_block.h>
#include <gr_tags.h>
#include <gruel/pmt.h>
#include <gruel/thread.h>
#include <cstdio>

#include <gr_file_meta_sink.h>

class gr_file_meta_source;
typedef boost::shared_ptr<gr_file_meta_source> gr_file_meta_source_sptr;

GR_CORE_API gr_file_meta_source_sptr
gr_make_file_meta_source(const char *filename,
			 bool repeat=false,
			 bool detached_header=false,
			 const char *hdr_filename="");

/*!
 * \brief Reads stream from file with meta-data headers. Headers are
 * parsed into tags.
 * \ingroup source_blk
 *
 * The information in the metadata headers includes:
 *
 *   rx_rate (double): sample rate of data.
 *   rx_time (uint64_t, double): time stamp of first sample in segment.
 *   type (gr_file_types as int32_t): data type.
 *   cplx (bool): Is data complex?
 *   strt (uint64_t): Starting byte of data in this segment.
 *   size (uint64_t): Size in bytes of data in this segment.
 *
 * Any item inside of the extra header dictionary is ready out and
 * made into a stream tag.
 */
class GR_CORE_API gr_file_meta_source : public gr_sync_block
{
  /*!
   * \brief Create a meta-data file source.
   *
   * \param filename (string): Name of file to write data to.
   * \param repeate (bool): Repeats file when EOF is found.
   * \param detached_header (bool): Set to true if header
   *    info is stored in a separate file (usually named filename.hdr)
   * \param hdr_filename (string): Name of detached header file if used.
   */
  friend GR_CORE_API gr_file_meta_source_sptr
    gr_make_file_meta_source(const char *filename,
			     bool repeat,
			     bool detached_header,
			     const char *hdr_filename);

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

  gruel::mutex d_mutex;
  FILE *d_new_fp, *d_new_hdr_fp;
  FILE *d_fp, *d_hdr_fp;
  meta_state_t d_state;

  std::vector<gr_tag_t> d_tags;

 protected:
  gr_file_meta_source(const char *filename,
		      bool repeat=false,
		      bool detached_header=false,
		      const char *hdr_filename="");

  bool _open(FILE **fp, const char *filename);
  bool read_header(pmt_t &hdr, pmt_t &extras);
  void parse_header(pmt_t hdr, uint64_t offset,
		    std::vector<gr_tag_t> &tags);

 public:
  ~gr_file_meta_source();

  bool open(const char *filename);
  void close();
  void do_update();

  int work(int noutput_items,
	   gr_vector_const_void_star &input_items,
	   gr_vector_void_star &output_items);
};

#endif /* INCLUDED_GR_FILE_META_SOURCE_H */
