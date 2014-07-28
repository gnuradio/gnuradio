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

#ifndef INCLUDED_ANALOG_PWR_SQUELCH_FF_IMPL_H
#define INCLUDED_ANALOG_PWR_SQUELCH_FF_IMPL_H

#include <gnuradio/analog/pwr_squelch_ff.h>
#include "squelch_base_ff_impl.h"
#include <gnuradio/filter/single_pole_iir.h>
#include <cmath>

namespace gr {
  namespace analog {

    class ANALOG_API pwr_squelch_ff_impl :
      public pwr_squelch_ff, squelch_base_ff_impl
    {
    private:
      double d_threshold;
      double d_pwr;
      filter::single_pole_iir<double,double,double> d_iir;

    protected:
      virtual void update_state(const float &in);
      virtual bool mute() const { return d_pwr < d_threshold; }

    public:
      pwr_squelch_ff_impl(double db, double alpha=0.0001,
			  int ramp=0, bool gate=false);
      ~pwr_squelch_ff_impl();

      std::vector<float> squelch_range() const;

      double threshold() const { return 10*log10(d_threshold); }
      void set_threshold(double db) { d_threshold = std::pow(10.0, db/10); }
      void set_alpha(double alpha) { d_iir.set_taps(alpha); }

      int ramp() const { return squelch_base_ff_impl::ramp(); }
      void set_ramp(int ramp) { squelch_base_ff_impl::set_ramp(ramp); }
      bool gate() const { return squelch_base_ff_impl::gate(); }
      void set_gate(bool gate) { squelch_base_ff_impl::set_gate(gate); }
      bool unmuted() const { return squelch_base_ff_impl::unmuted(); }

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items)
      {
	return squelch_base_ff_impl::general_work(noutput_items,
						  ninput_items,
						  input_items,
						  output_items);
      }
    };

  } /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_PWR_SQUELCH_FF_IMPL_H */
