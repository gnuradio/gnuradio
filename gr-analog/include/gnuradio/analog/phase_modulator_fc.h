/* -*- c++ -*- */
/*
 * Copyright 2005,2006,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_ANALOG_PHASE_MODULATOR_FC_H
#define INCLUDED_ANALOG_PHASE_MODULATOR_FC_H

#include <gnuradio/analog/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace analog {

    /*!
     * \brief Phase modulator block
     * \ingroup modulators_blk
     *
     * \details
     * output = complex(cos(in*sensitivity), sin(in*sensitivity))
     *
     * Input stream 0: floats
     * Output stream 0: complex
     */
    class ANALOG_API phase_modulator_fc : virtual public sync_block
    {
    public:
      // gr::analog::phase_modulator_fc::sptr
      typedef boost::shared_ptr<phase_modulator_fc> sptr;

      /* \brief Make a phase modulator block.
       *
       * \param sensitivity Phase change sensitivity of input amplitude.
       */
      static sptr make(double sensitivity);

      virtual double sensitivity() const = 0;
      virtual double phase() const = 0;

      virtual void set_sensitivity(double s) = 0;
      virtual void set_phase(double p) = 0;
    };

  } /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_PHASE_MODULATOR_FC_H */
