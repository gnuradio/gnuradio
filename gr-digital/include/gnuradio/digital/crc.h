/* -*- c++ -*- */
/*
 * Copyright 2022 Daniel Estevez <daniel@destevez.net>
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_CRC_H
#define INCLUDED_DIGITAL_CRC_H

#include <gnuradio/digital/api.h>

#include <stdint.h>
#include <array>
#include <vector>

namespace gr {
namespace digital {

/*!
 * \brief Calculates a CRC
 * \ingroup packet_operators_blk
 *
 * \details
 * This class calculates a CRC with configurable parameters.
 * A table-driven byte-by-byte approach is used in the CRC
 * computation.
 */
class DIGITAL_API crc
{
public:
    /*!
     * \brief Construct a CRC calculator instance.
     *
     * \param num_bits CRC size in bits
     * \param poly CRC polynomial, in MSB-first notation
     * \param initial_value Initial register value
     * \param final_xor Final XOR value
     * \param input_reflected true if the input is LSB-first, false if not
     * \param result_reflected true if the output is LSB-first, false if not
     */
    crc(unsigned num_bits,
        uint64_t poly,
        uint64_t initial_value,
        uint64_t final_xor,
        bool input_reflected,
        bool result_reflected);
    ~crc();

    /*!
     * \brief Computes a CRC
     *
     * \param data the input data for the CRC calculation
     * \param len the length in bytes of the data
     */
    uint64_t compute(const uint8_t* data, std::size_t len);

    /*!
     * \brief Computes a CRC
     *
     * \param data the input data for the CRC calculation
     */
    uint64_t compute(std::vector<uint8_t> const& data)
    {
        return compute(data.data(), data.size());
    }

private:
    std::array<uint64_t, 256> d_table;
    unsigned d_num_bits;
    uint64_t d_mask;
    uint64_t d_initial_value;
    uint64_t d_final_xor;
    bool d_input_reflected;
    bool d_result_reflected;

    uint64_t reflect(uint64_t word) const;
};

} // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_CRC_H */
