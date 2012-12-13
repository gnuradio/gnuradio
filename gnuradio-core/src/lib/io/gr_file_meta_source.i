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

GR_SWIG_BLOCK_MAGIC(gr,file_meta_source)

gr_file_meta_source_sptr
gr_make_file_meta_source(const char *filename,
			 bool repeat=false,
			 bool detached_header=false,
			 const char *hdr_filename="");

class gr_file_meta_source : public gr_sync_block
{
 protected:
  gr_file_meta_source(const char *filename,
		      bool repeat,
		      bool detached_header,
		      const char *hdr_filename);

 public:
  ~gr_file_meta_source();

  bool open(const char *filename);
  void close();
};
