/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef INCLUDED_GR_FILE_SINK_H
#define INCLUDED_GR_FILE_SINK_H

#include <gr_sync_block.h>
#include <omnithread.h>

class gr_file_sink;
typedef boost::shared_ptr<gr_file_sink> gr_file_sink_sptr;

gr_file_sink_sptr gr_make_file_sink(size_t itemsize, const char *filename);

/*!
 * \brief Write stream to file.
 * \ingroup sink
 */

class gr_file_sink : public gr_sync_block
{
  friend gr_file_sink_sptr gr_make_file_sink(size_t itemsize, const char *filename);

 private:
  size_t	d_itemsize;
  void	       *d_fp;		// current FILE pointer
  void	       *d_new_fp;	// new FILE pointer
  bool		d_updated;	// is there a new FILE pointer?
  omni_mutex	d_mutex;

 protected:
  gr_file_sink(size_t itemsize, const char *filename);

 public:
  ~gr_file_sink();

  /*! 
   * \brief Open filename and begin output to it.
   */
  bool open(const char *filename);

  /*!
   * \brief Close current output file.
   *
   * Closes current output file and ignores any output until
   * open is called to connect to another file.
   */
  void close();

  int work(int noutput_items,
	   gr_vector_const_void_star &input_items,
	   gr_vector_void_star &output_items);
};


#endif /* INCLUDED_GR_FILE_SINK_H */
