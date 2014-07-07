/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_BLOCKS_SHORT_TO_CHAR_H
#define INCLUDED_BLOCKS_SHORT_TO_CHAR_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief Convert stream of shorts to a stream of chars.
     *
     * This block strips off the least significant byte from the
     * short value.
     *
     * [0x00ff, 0x0ff0, 0xff00] => [0x00, 0x0f, 0xff]
     *
     * \ingroup type_converters_blk
     *
     * \details
     * Converts \p vlen length vectors of input short samples to chars,
     * dividing each element by 256.
     */
    class BLOCKS_API short_to_char : virtual public sync_block
    {
    public:
      // gr::blocks::short_to_char_ff::sptr
      typedef boost::shared_ptr<short_to_char> sptr;

      /*!
       * Build a short to char block.
       *
       * \param vlen vector length of data streams.
       */
      static sptr make(size_t vlen=1);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_SHORT_TO_CHAR_H */
