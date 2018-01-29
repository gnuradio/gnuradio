/* -*- c++ -*- */
/*
 * Copyright 2008,2010,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_LFSR_H
#define INCLUDED_DIGITAL_LFSR_H

#include <gnuradio/digital/api.h>
#include <stdexcept>
#include <stdint.h>

namespace gr {
  namespace digital {

    /*!
     * \brief Fibonacci Linear Feedback Shift Register using specified
     * polynomial mask
     * \ingroup misc
     *
     * \details
     * Generates a maximal length pseudo-random sequence of length
     * 2^degree-1
     *
     * Constructor: digital::lfsr(int mask, int seed, int reg_len);
     *
     * \param mask - polynomial coefficients representing the
     *             locations of feedback taps from a shift register
     *             which are xor'ed together to form the new high
     *             order bit.
     *
     *             Some common masks might be:
     *              x^4 + x^3 + x^0 = 0x19
     *              x^5 + x^3 + x^0 = 0x29
     *              x^6 + x^5 + x^0 = 0x61
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
      uint32_t d_shift_register;
      uint32_t d_mask;
      uint32_t d_seed;
      uint32_t d_shift_register_length;	// less than 32

      static uint32_t
	popCount(uint32_t x)
      {
	uint32_t r = x - ((x >> 1) & 033333333333)
	  - ((x >> 2) & 011111111111);
	return ((r + (r >> 3)) & 030707070707) % 63;
      }

    public:
      lfsr(uint32_t mask, uint32_t seed, uint32_t reg_len)
	: d_shift_register(seed),
	d_mask(mask),
	d_seed(seed),
	d_shift_register_length(reg_len)
	{
	  if(reg_len > 31)
	    throw std::invalid_argument("reg_len must be <= 31");
	}

      unsigned char next_bit()
      {
	unsigned char output = d_shift_register & 1;
	unsigned char newbit = popCount( d_shift_register & d_mask )%2;
	d_shift_register = ((d_shift_register>>1) | (newbit<<d_shift_register_length));
	return output;
      }

      unsigned char next_bit_scramble(unsigned char input)
      {
	unsigned char output = d_shift_register & 1;
	unsigned char newbit = (popCount( d_shift_register & d_mask )%2)^(input & 1);
	d_shift_register = ((d_shift_register>>1) | (newbit<<d_shift_register_length));
	return output;
      }

      unsigned char next_bit_descramble(unsigned char input)
      {
	unsigned char output = (popCount( d_shift_register & d_mask )%2)^(input & 1);
	unsigned char newbit = input & 1;
	d_shift_register = ((d_shift_register>>1) | (newbit<<d_shift_register_length));
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
	for(int i=0; i<num; i++) {
	  next_bit();
	}
      }

      int mask() const { return d_mask; }
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_LFSR_H */
