/* -*- c++ -*- */
/*
 * Copyright 2005,2011,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_CRC32_H
#define INCLUDED_DIGITAL_CRC32_H

#include <gnuradio/digital/api.h>
#include <gnuradio/types.h>
#include <string>

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
update_crc32(unsigned int crc, const unsigned char* buf, size_t len);

DIGITAL_API unsigned int update_crc32(unsigned int crc, const std::string buf);

DIGITAL_API unsigned int crc32(const unsigned char* buf, size_t len);

DIGITAL_API unsigned int crc32(const std::string buf);

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_CRC32_H */
