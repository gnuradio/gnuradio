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
      typedef boost::shared_ptr<additive_scrambler_bb> sptr;

      /*!
       * \brief Create additive scrambler.
       *
       * \param mask   Polynomial mask for LFSR
       * \param seed   Initial shift register contents
       * \param len    Shift register length
       * \param count  Number of bytes after which shift register is reset, 0=never
       * \param bits_per_byte Number of bits per byte
       * \param reset_tag_key When a tag with this key is detected, the shift register is reset (when this is set, count is ignored!)
       */
      static sptr make(int mask, int seed, int len, int count=0, int bits_per_byte=1, const std::string &reset_tag_key="");

      virtual int mask() const = 0;
      virtual int seed() const = 0;
      virtual int len() const = 0;
      virtual int count() const = 0;
      virtual int bits_per_byte() = 0;
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_ADDITIVE_SCRAMBLER_BB_H */
