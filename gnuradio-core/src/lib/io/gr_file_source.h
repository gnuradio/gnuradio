/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_FILE_SOURCE_H
#define INCLUDED_GR_FILE_SOURCE_H

#include <gr_core_api.h>
#include <gr_sync_block.h>

class gr_file_source;
typedef boost::shared_ptr<gr_file_source> gr_file_source_sptr;

GR_CORE_API gr_file_source_sptr
gr_make_file_source (size_t itemsize, const char *filename, bool repeat = false);

/*!
 * \brief Read stream from file
 * \ingroup source_blk
 */

class GR_CORE_API gr_file_source : public gr_sync_block
{
  friend GR_CORE_API gr_file_source_sptr gr_make_file_source (size_t itemsize,
						  const char *filename,
						  bool repeat);
 private:
  size_t	d_itemsize;
  void	       *d_fp;
  bool		d_repeat;

 protected:
  gr_file_source (size_t itemsize, const char *filename, bool repeat);

 public:
  ~gr_file_source ();

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);

  /*!
   * \brief seek file to \p seek_point relative to \p whence
   *
   * \param seek_point	sample offset in file
   * \param whence	one of SEEK_SET, SEEK_CUR, SEEK_END (man fseek)
   */
  bool seek (long seek_point, int whence);
};

#endif /* INCLUDED_GR_FILE_SOURCE_H */
