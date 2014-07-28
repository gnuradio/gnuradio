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

#ifndef INCLUDED_BLOCKS_INTERLEAVED_SHORT_TO_COMPLEX_H
#define INCLUDED_BLOCKS_INTERLEAVED_SHORT_TO_COMPLEX_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_decimator.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief Convert stream of interleaved shorts to a stream of complex
     * \ingroup type_converters_blk
     */
    class BLOCKS_API interleaved_short_to_complex : virtual public sync_decimator
    {
    public:
      // gr::blocks::interleaved_short_to_complex::sptr
      typedef boost::shared_ptr<interleaved_short_to_complex> sptr;

      /*!
       * Build an interleaved short to complex block.
       */
      static sptr make(bool vector_input=false, bool swap=false);

      virtual void set_swap(bool swap)=0;
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_INTERLEAVED_SHORT_TO_COMPLEX_H */
