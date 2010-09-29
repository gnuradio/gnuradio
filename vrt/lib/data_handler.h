/* -*- c++ -*- */
/*
 * Copyright 2008,2009,2010 Free Software Foundation, Inc.
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
#ifndef INCLUDED_VRT_DATA_HANDLER_H
#define INCLUDED_VRT_DATA_HANDLER_H

#include <stdint.h>
#include <stddef.h>

namespace vrt {

  /*!
   * \brief Abstract function object called to handle received data blocks.
   */
  class data_handler 
  {
  public:

    /*!
     * \param base points to the beginning of the data.
     * \param len is the length of the data in bytes.
     * \returns true if it wants to be called again, else false.
     */
    virtual bool operator()(const void *base, size_t len) = 0;
    virtual ~data_handler();
  };

} // namespace vrt

#endif /* INCLUDED_VRT_DATA_HANDLER_H */
