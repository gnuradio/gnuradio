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
#ifndef INCLUDED_COPY_HANDLER_H
#define INCLUDED_COPY_HANDLER_H

#include <usrp2/data_handler.h>
#include <boost/utility.hpp>

namespace usrp2 {

  class copy_handler : public data_handler, boost::noncopyable 
  {
    uint8_t *d_dest;      // next write pointer
    size_t   d_space;     // space left in destination buffer
    size_t   d_bytes;     // total bytes copied
    size_t   d_times;     // number of times invoked

  public:
    copy_handler(void *dest, size_t len);
    ~copy_handler();
 
    virtual data_handler::result operator()(const void *base, size_t len);

    size_t bytes() const { return d_bytes; }
    size_t times() const { return d_times; }

    static const size_t MIN_COPY_LEN = 1484; // FIXME: calculate eth packet - thdr
    bool full() const { return d_space < MIN_COPY_LEN; }
  };
    
} // namespace usrp2

#endif /* INCLUDED_COPY_HANDLER_H */
