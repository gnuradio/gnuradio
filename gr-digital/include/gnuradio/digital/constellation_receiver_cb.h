/* -*- c++ -*- */
/*
 * Copyright 2011,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_CONSTELLATION_RECEIVER_CB_H
#define	INCLUDED_DIGITAL_CONSTELLATION_RECEIVER_CB_H

#include <gnuradio/digital/api.h>
#include <gnuradio/digital/constellation.h>
#include <gnuradio/block.h>

namespace gr {
  namespace digital {

    /*!
     * \brief This block makes hard decisions about the received
     * symbols (using a constellation object) and also fine tunes
     * phase synchronization.
     *
     * \details
     *
     * The phase and frequency synchronization are based on a Costas
     * loop that finds the error of the incoming signal point compared
     * to its nearest constellation point. The frequency and phase of
     * the NCO are updated according to this error.
     */
    class DIGITAL_API constellation_receiver_cb
      : virtual public block
    {
    public:
      // gr::digital::constellation_receiver_cb::sptr
      typedef boost::shared_ptr<constellation_receiver_cb> sptr;

      /*!
       * \brief Constructor to synchronize incoming M-PSK symbols
       *
       * \param constellation constellation of points for generic modulation
       * \param loop_bw	Loop  bandwidth of the Costas Loop (~ 2pi/100)
       * \param fmin          minimum normalized frequency value the loop can achieve
       * \param fmax          maximum normalized frequency value the loop can achieve
       *
       * The constructor chooses which phase detector and decision
       * maker to use in the work loop based on the value of M.
       */
      static sptr make(constellation_sptr constellation, 
		       float loop_bw, float fmin, float fmax);

      virtual void phase_error_tracking(float phase_error) = 0;
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_CONSTELLATION_RECEIVER_CB_H */
