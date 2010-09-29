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
#ifndef INCLUDED_VRT_HEADER_UTILS_H
#define INCLUDED_VRT_HEADER_UTILS_H

#include <iosfwd>
#include <string>
#include <stdint.h>
#include <vrt/bits.h>

namespace vrt 
{
  namespace detail {

    void wr_header(std::ostream &os, uint32_t x);
    void wr_name(std::ostream &os, const std::string &x);
    void wr_int_secs(std::ostream &os, uint32_t secs);
    void wr_uint32_hex(std::ostream &os, uint32_t x);
    void wr_uint32_dec(std::ostream &os, uint32_t x);
    void wr_hertz(std::ostream &os, vrt_hertz_t x);
    void wr_dbm(std::ostream &os, vrt_db_t x);
    void wr_db(std::ostream &os, vrt_db_t x);
    void wr_temp(std::ostream &os, vrt_temp_t x);
    void wr_angle(std::ostream &os, vrt_angle_t x);
    void wr_distance(std::ostream &os, vrt_distance_t x);
    void wr_velocity(std::ostream &os, vrt_velocity_t x);
    void wr_payload_fmt(std::ostream &os, vrt_payload_fmt_t f);
    void wr_formatted_gps(std::ostream &os,
			  const vrt_formatted_gps_t &x);
  };
};

#endif /* INCLUDED_VRT_HEADER_UTILS_H */
