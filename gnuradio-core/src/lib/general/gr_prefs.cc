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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gr_prefs.h>

/*
 * Stub implementations
 */

static gr_prefs	 s_default_singleton;
static gr_prefs  *s_singleton = &s_default_singleton;

gr_prefs *
gr_prefs::singleton()
{
  return s_singleton;
}

void
gr_prefs::set_singleton(gr_prefs *p)
{
  s_singleton = p;
}

gr_prefs::~gr_prefs()
{
  // nop
}

bool
gr_prefs::has_section(const std::string section)
{
  return false;
}

bool
gr_prefs::has_option(const std::string section, const std::string option)
{
  return false;
}

const std::string
gr_prefs::get_string(const std::string section, const std::string option, const std::string default_val)
{
  return default_val;
}

bool
gr_prefs::get_bool(const std::string section, const std::string option, bool default_val)
{
  return default_val;
}

long
gr_prefs::get_long(const std::string section, const std::string option, long default_val)
{
  return default_val;
}

double
gr_prefs::get_double(const std::string section, const std::string option, double default_val)
{
  return default_val;
}

