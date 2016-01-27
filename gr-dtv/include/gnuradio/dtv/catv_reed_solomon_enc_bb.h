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

#ifndef INCLUDED_DTV_CATV_REED_SOLOMON_ENC_BB_H
#define INCLUDED_DTV_CATV_REED_SOLOMON_ENC_BB_H

#include <gnuradio/dtv/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace dtv {

    /*!
     * \brief Reed Solomon Encoder, t=3, (128,122), 7-bit symbols.
     * \ingroup dtv
     *
     * Input: MPEG-2 bitstream packets of 122 7-bit symbols.\n
     * Output: MPEG-2 + RS parity bitstream packets of 128 7-bit symbols.
     */
    class DTV_API catv_reed_solomon_enc_bb : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<catv_reed_solomon_enc_bb> sptr;

      /*!
       * \brief Create an ITU-T J.83B Reed Solomon encoder.
       *
       */
      static sptr make();
    };

  } // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_CATV_REED_SOLOMON_ENC_BB_H */

