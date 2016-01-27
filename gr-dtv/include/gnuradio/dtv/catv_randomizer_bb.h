/* -*- c++ -*- */
/* 
 * Copyright 2016 Free Software Foundation, Inc.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_DTV_CATV_RANDOMIZER_BB_H
#define INCLUDED_DTV_CATV_RANDOMIZER_BB_H

#include <gnuradio/dtv/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace dtv {

    /*!
     * \brief Randomizer, x^3 + x + alpha^3, 7-bit symbols.
     * \ingroup dtv
     *
     * Input: Interleaved MPEG-2 + RS parity bitstream packets of 128 7-bit symbols.\n
     * Output: Scrambled FEC Frame packets of 60 * 128 7-bit symbols.
     */
    class DTV_API catv_randomizer_bb : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<catv_randomizer_bb> sptr;

      /*!
       * \brief Create an ITU-T J.83B randomizer.
       *
       */
      static sptr make();
    };

  } // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_CATV_RANDOMIZER_BB_H */

