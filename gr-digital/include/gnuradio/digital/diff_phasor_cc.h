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

#ifndef INCLUDED_GR_DIFF_PHASOR_CC_H
#define INCLUDED_GR_DIFF_PHASOR_CC_H

#include <gnuradio/digital/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace digital {

    /*!
     * \brief Differential decoding based on phase change.
     * \ingroup symbol_coding_blk
     *
     * \details
     * Uses the phase difference between two symbols to determine the
     * output symbol:
     *
     *     out[i] = in[i] * conj(in[i-1]);
     */
    class DIGITAL_API diff_phasor_cc : virtual public sync_block
    {
    public:
      // gr::digital::diff_phasor_cc::sptr
      typedef boost::shared_ptr<diff_phasor_cc> sptr;

      /*!
       * Make a differential phasor decoding block.
       */
      static sptr make();
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_GR_DIFF_PHASOR_CC_H */
