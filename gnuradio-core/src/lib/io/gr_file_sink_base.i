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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
class gr_file_sink_base 
{
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

  /*!
   *\brief turn on unbuffered mode for slow outputs
   */
  void set_unbuffered(bool unbuffered);
};
