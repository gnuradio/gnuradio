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

#ifndef INCLUDED_ANALOG_FEEDFORWARD_AGC_CC_H
#define INCLUDED_ANALOG_FEEDFORWARD_AGC_CC_H

#include <gnuradio/analog/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace analog {

    /*!
     * \brief Non-causal AGC which computes required gain based on max
     * absolute value over nsamples
     * \ingroup level_controllers_blk
     */
    class ANALOG_API feedforward_agc_cc : virtual public sync_block
    {
    public:
      // gr::analog::feedforward_agc_cc::sptr
      typedef boost::shared_ptr<feedforward_agc_cc> sptr;

      /*!
       * Build a complex valued feed-forward AGC loop block.
       *
       * \param nsamples number of samples to look ahead.
       * \param reference reference value to adjust signal power to.
       */
      static sptr make(int nsamples, float reference);
    };

  } /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_GR_FEEDFORWARD_AGC_CC_H */
