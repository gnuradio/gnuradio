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

#include <digital/api.h>
#include <gr_sync_block.h>

namespace gr {
  namespace digital {
    
    /*!
     * \ingroup coding_blk
     *
     * Scramble an input stream using an LFSR. This block works on the
     * LSB only of the input data stream, i.e., on an "unpacked
     * binary" stream, and produces the same format on its output.
     */
    class DIGITAL_API additive_scrambler_bb : virtual public gr_sync_block
    {
    public:
      // gr::digital::additive_scrambler_bb::sptr
      typedef boost::shared_ptr<additive_scrambler_bb> sptr;

      /*!
       * \brief Create additive scrambler.
       *
       * Scramble an input stream using an LFSR. This block works on
       * the LSB only of the input data stream, i.e., on an "unpacked
       * binary" stream, and produces the same format on its output.
       *
       * \param mask   Polynomial mask for LFSR
       * \param seed   Initial shift register contents
       * \param len    Shift register length
       * \param count  Number of bits after which shift register is reset, 0=never
       *
       * The scrambler works by XORing the incoming bit stream by the
       * output of the LFSR. Optionally, after 'count' bits have been
       * processed, the shift register is reset to the seed value.
       * This allows processing fixed length vectors of samples.
       */
      static sptr make(int mask, int seed, int len, int count=0);

      virtual int mask() const = 0;
      virtual int seed() const = 0;
      virtual int len() const = 0;
      virtual int count() const = 0;
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_ADDITIVE_SCRAMBLER_BB_H */
