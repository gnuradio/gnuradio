/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
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
#ifndef INCLUDED_GRI_LOGGER_H
#define INCLUDED_GRI_LOGGER_H

#include <stddef.h>
#include <omnithread.h>
#include <gr_buffer.h>

class gri_log_poster;
class gri_logger;
typedef boost::shared_ptr<gri_logger> gri_logger_sptr;


/*!
 * \brief non-blocking logging to a file.
 *
 * In reality, this may block, but only for a bounded time.
 * Trust me, it's safe to use from portaudio and JACK callbacks.
 */
class gri_logger
{
  gri_log_poster	*d_poster;
  omni_mutex		 d_write_mutex;

public:
  static gri_logger_sptr singleton();

  gri_logger(const char *filename);
  ~gri_logger();

  void write(const void *buf, size_t count);
  void printf(const char *format, ...);
};

#endif /* INCLUDED_GRI_LOGGER_H */
