/* -*- c++ -*- */
/*
 * Copyright 2010 Free Software Foundation, Inc.
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

#include "header_utils.h"
#include <ostream>
#include <time.h>
#include <boost/format.hpp>

using boost::format;
using boost::io::group;

namespace vrt
{
  namespace detail {

    void wr_int_secs(std::ostream &os, uint32_t secs)
    {
      os << format("%10d") % secs;

      time_t t = secs;
      struct tm r;

      if (gmtime_r(&t, &r)){
	// ISO 8601 date + time
	os << format("  %04d-%02d-%02d %02d:%02d:%02dZ")
	  % (r.tm_year + 1900) % (r.tm_mon + 1) % r.tm_mday
	  % r.tm_hour % r.tm_min % r.tm_sec;
      }

      os << std::endl;
    }

    void wr_name(std::ostream &os, const std::string &x)
    {
      os << format("  %-21s ") % (x + ":");
    }

    void wr_uint32_hex(std::ostream &os, uint32_t x)
    {
      os << format("%#10x") % x;
      os << std::endl;
    }

    void wr_uint32_dec(std::ostream &os, uint32_t x)
    {
      os << format("%12d") % x;
      os << std::endl;
    }

  };
};
