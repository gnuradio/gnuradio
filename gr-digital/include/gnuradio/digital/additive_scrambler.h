/* -*- c++ -*- */
/*
 * Copyright 2008,2010,2012 Free Software Foundation, Inc.
 * Copyright 2024 Daniel Estevez <daniel@destevez.net>
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_ADDITIVE_SCRAMBLER_H
#define INCLUDED_DIGITAL_ADDITIVE_SCRAMBLER_H

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
 *
 * The scrambler can work with soft symbols (gr_complex, float,
 * int32_t, or int16_t) or with hard symbols (uint8_t). When working
 * with soft symbols, the scrambler works by changing the sign of the
 * input symbol whenever the output of the LFSR is one.
 *
 * When working with hard symbols, this block scrambles up to
 * 8 bits per byte of the input data stream, starting at the LSB.
 * In this case, the scrambler works by XORing the incoming bit stream
 * by the output of the LFSR.
 *
 * In both cases, optionally, after \p count bits have been
 * processed, the shift register is reset to the \p seed value.
 * This allows processing fixed length vectors of samples.
 *
 * Alternatively, the LFSR can be reset using a reset tag to
 * scramble variable length vectors. However, it cannot be reset
 * between bytes in the hard symbol mode.
 *
 * For details on configuring the LFSR, see gr::digital::lfsr.
 */
template <class T>
class DIGITAL_API additive_scrambler : virtual public sync_block
{
public:
    typedef std::shared_ptr<additive_scrambler<T>> sptr;

    /*!
     * \brief Create additive scrambler.
     *
     * \param mask   Polynomial mask for LFSR
     * \param seed   Initial shift register contents
     * \param len    Shift register length
     * \param count  Number of bytes after which shift register is reset, 0=never
     * \param bits_per_byte Number of bits per byte (must be 1 if T != uint8_t)
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

typedef additive_scrambler<gr_complex> additive_scrambler_cc;
typedef additive_scrambler<float> additive_scrambler_ff;
typedef additive_scrambler<int32_t> additive_scrambler_ii;
typedef additive_scrambler<int16_t> additive_scrambler_ss;
typedef additive_scrambler<uint8_t> additive_scrambler_bb;

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_ADDITIVE_SCRAMBLER_H */
