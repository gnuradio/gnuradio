/* -*- c++ -*- */
/*
 * Copyright 2008,2010,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_LFSR_H
#define INCLUDED_DIGITAL_LFSR_H

#include <gnuradio/digital/api.h>
#ifndef HAVE_BUILTIN_PARITYL
#include <volk/volk.h>
#endif
#include <stdint.h>
#include <cstdint>
#include <stdexcept>

namespace gr {
namespace digital {

/*!
 * \brief Fibonacci Linear Feedback Shift Register using specified
 * polynomial mask
 * \ingroup misc
 *
 * \details
 * Generates a maximal length pseudo-random sequence of length
 * 2^degree-1, if supplied with a primitive polynomial.
 *
 * Constructor: digital::lfsr(int mask, int seed, int reg_len);
 *
 * \param mask - polynomial coefficients representing the
 *             locations of feedback taps from a shift register
 *             which are xor'ed together to form the new high
 *             order bit.
 *
 *             Some common masks might be:
 *              x^4 + x^3 + x^0 = 0x19, K=3
 *              x^5 + x^3 + x^0 = 0x29, K=4
 *              x^6 + x^5 + x^0 = 0x61, K=5
 *
 * \param seed - the initialization vector placed into the
 *             register during initialization. Low order bit
 *             corresponds to x^0 coefficient -- the first to be
 *             shifted as output.
 *
 * \param reg_len - specifies the length of the feedback shift
 *             register to be used. During each iteration, the
 *             register is rightshifted one and the new bit is
 *             placed in bit reg_len. reg_len should generally be
 *             at least order(mask) + 1
 *
 *
 * see http://en.wikipedia.org/wiki/Linear_feedback_shift_register
 * for more explanation.
 *
 *  next_bit() - Standard LFSR operation
 *
 *      Perform one cycle of the LFSR.  The output bit is taken from
 *      the shift register LSB.  The shift register MSB is assigned from
 *      the modulo 2 sum of the masked shift register.
 *
 *  next_bit_scramble(unsigned char input) - Scramble an input stream
 *
 *      Perform one cycle of the LFSR.  The output bit is taken from
 *      the shift register LSB.  The shift register MSB is assigned from
 *      the modulo 2 sum of the masked shift register and the input LSB.
 *
 *  next_bit_descramble(unsigned char input) - Descramble an input stream
 *
 *      Perform one cycle of the LFSR.  The output bit is taken from
 *      the modulo 2 sum of the masked shift register and the input LSB.
 *      The shift register MSB is assigned from the LSB of the input.
 *
 * See http://en.wikipedia.org/wiki/Scrambler for operation of these
 * last two functions (see multiplicative scrambler.)
 */
class lfsr
{
private:
    uint64_t d_shift_register;
    uint64_t d_mask;
    uint64_t d_seed;
    uint8_t d_shift_register_length; // less than 64

public:
    lfsr(uint64_t mask, uint64_t seed, uint8_t reg_len)
        : d_shift_register(seed),
          d_mask(mask),
          d_seed(seed),
          d_shift_register_length(reg_len)
    {
        if (reg_len > 63)
            throw std::invalid_argument("reg_len must be <= 63");
    }

    unsigned char next_bit()
    {
        unsigned char output = d_shift_register & 1;
        uint64_t newbit;
#ifdef HAVE_BUILTIN_PARITYL
        newbit = __builtin_parityl(d_shift_register & d_mask);
#else
        volk_64u_popcnt(&newbit, d_shift_register & d_mask);
        newbit %= 2;
#endif

        d_shift_register =
            ((d_shift_register >> 1) | (newbit << d_shift_register_length));
        return output;
    }

    unsigned char next_bit_scramble(unsigned char input)
    {
        unsigned char output = d_shift_register & 1;
        uint64_t newbit;
#ifdef HAVE_BUILTIN_PARITYL
        newbit = __builtin_parityl(d_shift_register & d_mask) ^ (input & 1);
#else
        volk_64u_popcnt(&newbit, d_shift_register & d_mask);
        newbit = (newbit ^ input) & 1;
#endif
        d_shift_register =
            ((d_shift_register >> 1) | (newbit << d_shift_register_length));
        return output;
    }

    unsigned char next_bit_descramble(unsigned char input)
    {
        unsigned char output;
#ifdef HAVE_BUILTIN_PARITYL
        output = __builtin_parityl(d_shift_register & d_mask) ^ (input & 1);
#else
        uint64_t _tmp;
        volk_64u_popcnt(&_tmp, d_shift_register & d_mask);
        output = (_tmp ^ input) & 1;
#endif
        uint64_t newbit = input & 1;
        d_shift_register =
            ((d_shift_register >> 1) | (newbit << d_shift_register_length));
        return output;
    }

    /*!
     * Reset shift register to initial seed value
     */
    void reset() { d_shift_register = d_seed; }

    /*!
     * Rotate the register through x number of bits
     * where we are just throwing away the results to get queued up correctly
     */
    void pre_shift(int num)
    {
        for (int i = 0; i < num; i++) {
            next_bit();
        }
    }

    uint64_t mask() const { return d_mask; }
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_LFSR_H */
