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
#ifndef INCLUDED_DATA_HANDLER_H
#define INCLUDED_DATA_HANDLER_H

#include <stdint.h>
#include <stddef.h>

namespace usrp2 {

  /*!
   * \brief Abstract function object called to handle received data blocks.
   */
  class data_handler 
  {
  public:

    enum result_bits {
      RELEASE	= 0x0000,	//< OK to release data (opposite of KEEP)
      KEEP	= 0x0001,	//< do not discard data
      DONE	= 0x0002,    	//< do not call this object again
    };
    
    typedef int result;		//< bitmask of result_bits

    /*!
     * \param base points to the beginning of the data
     * \param len is the length in bytes of the data
     * \returns bitmask composed of DONE, KEEP
     */
    virtual result operator()(const void *base, size_t len) = 0;
    virtual ~data_handler();
  };

} // namespace usrp2

#endif /* INCLUDED_DATA_HANDLER_H */
