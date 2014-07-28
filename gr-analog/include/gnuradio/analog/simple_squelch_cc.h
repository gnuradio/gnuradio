/* -*- c++ -*- */
/*
 * Copyright 2005,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_ANALOG_SIMPLE_SQUELCH_CC_H
#define INCLUDED_ANALOG_SIMPLE_SQUELCH_CC_H

#include <gnuradio/analog/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace analog {

    /*!
     * \brief simple squelch block based on average signal power and threshold in dB.
     * \ingroup level_controllers_blk
     */
    class ANALOG_API simple_squelch_cc : virtual public sync_block
    {
    public:
      // gr::analog::simple_squelch_cc::sptr
      typedef boost::shared_ptr<simple_squelch_cc> sptr;

      /*!
       * \brief Make a simple squelch block.
       *
       * \param threshold_db Threshold for muting.
       * \param alpha Gain parameter for the running average filter.
       */
      static sptr make(double threshold_db, double alpha);

      virtual bool unmuted() const  = 0;

      virtual void set_alpha(double alpha) = 0;
      virtual void set_threshold(double decibels) = 0;

      virtual double threshold() const = 0;
      virtual std::vector<float> squelch_range() const = 0;
    };

  } /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_SIMPLE_SQUELCH_CC_H */
