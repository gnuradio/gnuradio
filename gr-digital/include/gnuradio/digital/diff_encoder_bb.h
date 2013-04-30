/* -*- c++ -*- */
/*
 * Copyright 2006,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_DIFF_ENCODER_BB_H
#define INCLUDED_GR_DIFF_ENCODER_BB_H

#include <gnuradio/digital/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace digital {

    /*!
     * \brief Differential decoder: y[0] = (x[0] + y[-1]) % M
     * \ingroup symbol_coding_blk
     *
     * \details
     * Uses current and previous symbols and the alphabet modulus to
     * perform differential encoding.
     */
    class DIGITAL_API diff_encoder_bb : virtual public sync_block
    {
    public:
      // gr::digital::diff_encoder_bb::sptr
      typedef boost::shared_ptr<diff_encoder_bb> sptr;

      /*!
       * Make a differntial encoder block.
       *
       * \param modulus Modulus of code's alphabet
       */
      static sptr make(unsigned int modulus);
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_GR_DIFF_ENCODER_BB_H */
