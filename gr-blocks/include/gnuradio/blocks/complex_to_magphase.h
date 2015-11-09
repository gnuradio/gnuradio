/* -*- c++ -*- */
/*
 * Copyright 2005,2012,2015 Free Software Foundation, Inc.
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

#ifndef INCLUDED_BLOCKS_COMPLEX_TO_MAGPHASE_H
#define INCLUDED_BLOCKS_COMPLEX_TO_MAGPHASE_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief Convert a stream of gr_complex to magnitude and phase (both floats)
     * \ingroup type_converters_blk
     *
     * \details
     * output[0] is the magnitude
     * output[1] is the phase (in radians)
     */
    class BLOCKS_API complex_to_magphase : virtual public sync_block
    {
    public:
      // gr::blocks::complex_to_magphase_ff::sptr
      typedef boost::shared_ptr<complex_to_magphase> sptr;

      /*!
       * Build a complex to magnitude and phase block.
       *
       * \param vlen vector len (default 1)
       */
      static sptr make(size_t vlen=1);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_COMPLEX_TO_MAGPHASE_H */
