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

#ifndef INCLUDED_BLOCKS_COMPLEX_TO_INTERLEAVED_SHORT_H
#define INCLUDED_BLOCKS_COMPLEX_TO_INTERLEAVED_SHORT_H

#include <blocks/api.h>
#include <gr_sync_interpolator.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief Convert stream of complex to a stream of interleaved shorts
     * \ingroup converter_blk
     */
    class BLOCKS_API complex_to_interleaved_short : virtual public gr_sync_interpolator
    {
    public:
      
      // gr::blocks::complex_to_interleaved_short::sptr
      typedef boost::shared_ptr<complex_to_interleaved_short> sptr;

      static sptr make();
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_COMPLEX_TO_INTERLEAVED_SHORT_H */
