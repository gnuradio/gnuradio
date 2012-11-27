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

GR_SWIG_BLOCK_MAGIC(gr,file_meta_sink)

enum gr_file_types {
  GR_FILE_BYTE=0,
  GR_FILE_CHAR=0,
  GR_FILE_SHORT,
  GR_FILE_INT,
  GR_FILE_LONG,
  GR_FILE_LONG_LONG,
  GR_FILE_FLOAT,
  GR_FILE_DOUBLE,
};

gr_file_meta_sink_sptr
gr_make_file_meta_sink(size_t itemsize, const char *filename,
		       double samp_rate=1, double relative_rate=1,
		       gr_file_types type=GR_FILE_FLOAT, bool complex=true,
		       const std::string & extra_dict="");

class gr_file_meta_sink : public gr_sync_block, public gr_file_sink_base
{
 protected:
  gr_file_meta_sink(size_t itemsize, const char *filename,
		    double samp_rate, double relative_rate,
		    gr_file_types type, bool complex,
		    const std::string & extra_dict);

 public:
  ~gr_file_meta_sink();

  enum file_types {
    FILE_BYTE=0,
    FILE_CHAR=0,
    FILE_SHORT,
    FILE_INT,
    FILE_LONG,
    FILE_LONG_LONG,
    FILE_FLOAT,
    FILE_DOUBLE,
  };

  /*!
   * \brief open filename and begin output to it.
   */
  bool open(const char *filename);

  /*!
   * \brief close current output file.
   */
  void close();
};
