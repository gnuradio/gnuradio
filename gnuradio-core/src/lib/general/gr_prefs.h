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
#ifndef INCLUDED_GR_PREFS_H
#define INCLUDED_GR_PREFS_H

#include <gr_core_api.h>
#include <string>
#include <gruel/thread.h>

/*!
 * \brief Base class for representing user preferences a la windows INI files.
 * \ingroup misc
 *
 * The real implementation is in Python, and is accessable from C++
 * via the magic of SWIG directors.
 */

class GR_CORE_API gr_prefs
{
public:
  static gr_prefs *singleton();
  static void set_singleton(gr_prefs *p);

  gr_prefs();
  virtual ~gr_prefs();

  /*!
   * \brief Does \p section exist?
   */
  virtual bool has_section(const std::string &section);

  /*!
   * \brief Does \p option exist?
   */
  virtual bool has_option(const std::string &section, const std::string &option);

  /*!
   * \brief If option exists return associated value; else default_val.
   */
  virtual const std::string get_string(const std::string &section,
				       const std::string &option,
				       const std::string &default_val);

  /*!
   * \brief If option exists and value can be converted to bool, return it; else default_val.
   */
  virtual bool get_bool(const std::string &section,
			const std::string &option,
			bool default_val);

  /*!
   * \brief If option exists and value can be converted to long, return it; else default_val.
   */
  virtual long get_long(const std::string &section,
			const std::string &option,
			long default_val);

  /*!
   * \brief If option exists and value can be converted to double, return it; else default_val.
   */
  virtual double get_double(const std::string &section,
			    const std::string &option,
			    double default_val);

 protected:
  virtual std::vector<std::string> _sys_prefs_filenames();
  virtual void _read_files();

 private:
  gruel::mutex d_mutex;
  std::string d_configs;
};


#endif /* INCLUDED_GR_PREFS_H */
