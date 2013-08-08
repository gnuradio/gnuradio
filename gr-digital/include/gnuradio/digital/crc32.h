/* -*- c++ -*- */
/*
 * Copyright 2005,2011,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_CRC32_H
#define INCLUDED_DIGITAL_CRC32_H

#include <gnuradio/digital/api.h>
#include <string>
#include <gnuradio/types.h>

namespace gr {
  namespace digital {

    /*!
     * \brief update running CRC-32
     * \ingroup packet_operators_blk
     *
     * \details
     * Update a running CRC with the bytes buf[0..len-1] The CRC
     * should be initialized to all 1's, and the transmitted value is
     * the 1's complement of the final running CRC.  The resulting CRC
     * should be transmitted in big endian order.
     */
    DIGITAL_API unsigned int 
    update_crc32(unsigned int crc, const unsigned char *buf, size_t len);

    DIGITAL_API unsigned int 
    update_crc32(unsigned int crc, const std::string buf);

    DIGITAL_API unsigned int 
    crc32(const unsigned char *buf, size_t len);

    DIGITAL_API unsigned int 
    crc32(const std::string buf);

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_CRC32_H */
