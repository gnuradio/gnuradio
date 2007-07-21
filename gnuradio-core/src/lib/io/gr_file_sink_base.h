/* -*- c++ -*- */
/*
 * Copyright 2004,2007 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_FILE_SINK_BASE_H
#define INCLUDED_GR_FILE_SINK_BASE_H

#include <omnithread.h>
#include <cstdio>

/*!
 * \brief Common base class for file sinks
 */
class gr_file_sink_base 
{
 protected:
  FILE	       *d_fp;		// current FILE pointer
  FILE	       *d_new_fp;	// new FILE pointer
  bool		d_updated;	// is there a new FILE pointer?
  bool		d_is_binary;
  omni_mutex	d_mutex;

 protected:
  gr_file_sink_base(const char *filename, bool is_binary);

 public:
  ~gr_file_sink_base();

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

  /*!
   * \brief if we've had an update, do it now.
   */
  void do_update();
};


#endif /* INCLUDED_GR_FILE_SINK_BASE_H */
