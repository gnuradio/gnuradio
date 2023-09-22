/* -*- c++ -*- */
/*
 * Copyright 2022 Daniel Estevez <daniel@destevez.net>
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_CRC_CHECK_H
#define INCLUDED_DIGITAL_CRC_CHECK_H

#include <gnuradio/block.h>
#include <gnuradio/digital/api.h>

#include <stdint.h>

namespace gr {
namespace digital {

/*!
 * \brief Check the CRC at the end of a PDU
 * \ingroup packet_operators_blk
 *
 * \details
 * The CRC check block receives a PDU containing a CRC at its end,
 * and checks whether the CRC is correct. The PDU is sent over the ok
 * or fail output ports according to the result of this check.
 * It can support any CRC whose size is a multiple of 8 bits between
 * 8 and 64 bits.
 */
class DIGITAL_API crc_check : virtual public block
{
public:
    typedef std::shared_ptr<crc_check> sptr;

    /*!
     * \brief Build the CRC check block.
     *
     * \param num_bits CRC size in bits (must be a multiple of 8)
     * \param poly CRC polynomial, in MSB-first notation
     * \param initial_value Initial register value
     * \param final_xor Final XOR value
     * \param input_reflected true if the input is LSB-first, false if not
     * \param result_reflected true if the output is LSB-first, false if not
     * \param swap_endianness true if the CRC is stored as little-endian in the PDU,
       false if not
     * \param discard_crc If true, the CRC is removed from the PDU before sending
       it to the output port. If false, the CRC is preserved in the output PDU.
     * \param skip_header_bytes gives the number of header byte to skip in the CRC
       calculation
     */
    static sptr make(unsigned num_bits,
                     uint64_t poly,
                     uint64_t initial_value,
                     uint64_t final_xor,
                     bool input_reflected,
                     bool result_reflected,
                     bool swap_endianness,
                     bool discard_crc = false,
                     unsigned skip_header_bytes = 0);
};

} // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_CRC_CHECK_H */
