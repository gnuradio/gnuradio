/* -*- c++ -*- */
/*
 * Copyright 2008,2010,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_ADDITIVE_SCRAMBLER_BB_H
#define INCLUDED_DIGITAL_ADDITIVE_SCRAMBLER_BB_H

#include <gnuradio/digital/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace digital {

/*!
 * \ingroup coding_blk
 *
 * \brief
 * Scramble an input stream using an LFSR.
 *
 * \details
 * This block scrambles up to 8 bits per byte of the input
 * data stream, starting at the LSB.
 *
 * The scrambler works by XORing the incoming bit stream by the
 * output of the LFSR. Optionally, after \p count bits have been
 * processed, the shift register is reset to the \p seed value.
 * This allows processing fixed length vectors of samples.
 *
 * Alternatively, the LFSR can be reset using a reset tag to
 * scramble variable length vectors. However, it cannot be reset
 * between bytes.
 *
 * For details on configuring the LFSR, see gr::digital::lfsr.
 */
class DIGITAL_API additive_scrambler_bb : virtual public sync_block
{
public:
    // gr::digital::additive_scrambler_bb::sptr
    typedef std::shared_ptr<additive_scrambler_bb> sptr;

    /*!
     * \brief Create additive scrambler.
     *
     * \param mask   Polynomial mask for LFSR
     * \param seed   Initial shift register contents
     * \param len    Shift register length
     * \param count  Number of bytes after which shift register is reset, 0=never
     * \param bits_per_byte Number of bits per byte
     * \param reset_tag_key When a tag with this key is detected, the shift register is
     * reset (when this is set, count is ignored!)
     */
    static sptr make(uint64_t mask,
                     uint64_t seed,
                     uint8_t len,
                     int64_t count = 0,
                     uint8_t bits_per_byte = 1,
                     const std::string& reset_tag_key = "");

    virtual uint64_t mask() const = 0;
    virtual uint64_t seed() const = 0;
    virtual uint8_t len() const = 0;
    virtual int64_t count() const = 0;
    virtual uint8_t bits_per_byte() = 0;
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_ADDITIVE_SCRAMBLER_BB_H */
