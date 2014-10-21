/* -*- c++ -*- */
/*
 * Copyright 2004,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_TRELLIS_SISO_COMBINED_F_H
#define INCLUDED_TRELLIS_SISO_COMBINED_F_H

#include <gnuradio/trellis/api.h>
#include <gnuradio/trellis/fsm.h>
#include <gnuradio/trellis/siso_type.h>
#include <gnuradio/trellis/calc_metric.h>
#include <gnuradio/trellis/core_algorithms.h>
#include <gnuradio/block.h>

namespace gr {
  namespace trellis {

    /*!
     *  \ingroup trellis_coding_blk
     */
    class TRELLIS_API siso_combined_f : virtual public block
    {
    public:
      // gr::trellis::siso_combined_f::sptr
      typedef boost::shared_ptr<siso_combined_f> sptr;

      static sptr make(const fsm &FSM, int K,
		       int S0, int SK,
		       bool POSTI, bool POSTO,
		       siso_type_t d_SISO_TYPE,
		       int D,
		       const std::vector<float> &TABLE,
		       digital::trellis_metric_type_t TYPE);

      virtual fsm FSM() const = 0;
      virtual int K() const = 0;
      virtual int S0() const = 0;
      virtual int SK() const = 0;
      virtual bool POSTI() const = 0;
      virtual bool POSTO() const = 0;
      virtual siso_type_t SISO_TYPE() const = 0;
      virtual int D() const = 0;
      virtual std::vector<float> TABLE() const = 0;
      virtual digital::trellis_metric_type_t TYPE() const = 0;
    };

  } /* namespace trellis */
} /* namespace gr */

#endif /* INCLUDED_TRELLIS_SISO_COMBINED_F_H */
