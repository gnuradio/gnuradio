/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <usrp2/copy_handler.h>
#include <iostream>
#include <string.h>

namespace usrp2 {
  
  copy_handler::copy_handler(void *dest, size_t len)
    : d_dest((uint8_t *)dest), d_space(len), d_bytes(0), d_times(0)
  {
  }
  
  copy_handler::~copy_handler()
  {
    // NOP
  }
  
  data_handler::result
  copy_handler::operator()(const void *base, size_t len)
  {
    if (len > d_space)
      return KEEP|DONE; // can't do anything, retry later
    
    memcpy(&d_dest[d_bytes], base, len);
    d_space -= len;
    d_bytes += len;
    d_times++;

    if (d_space < MIN_COPY_LEN)
      return DONE; // don't call me anymore
    
    return 0;
  }
  
} // namespace usrp2

