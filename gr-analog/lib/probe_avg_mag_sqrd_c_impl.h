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

#ifndef INCLUDED_ANALOG_PROBE_AVG_MAG_SQRD_C_IMPL_H
#define INCLUDED_ANALOG_PROBE_AVG_MAG_SQRD_C_IMPL_H

#include <gnuradio/analog/probe_avg_mag_sqrd_c.h>
#include <gnuradio/filter/single_pole_iir.h>

namespace gr {
  namespace analog {

    class probe_avg_mag_sqrd_c_impl : public probe_avg_mag_sqrd_c
    {
    private:
      double d_threshold;
      bool d_unmuted;
      double d_level;
      filter::single_pole_iir<double,double,double> d_iir;

    public:
      probe_avg_mag_sqrd_c_impl(double threshold_db, double alpha = 0.0001);
      ~probe_avg_mag_sqrd_c_impl();

      bool unmuted() const { return d_unmuted; }
      double level() const { return d_level; }

      double threshold() const;

      void set_alpha(double alpha);
      void set_threshold(double decibels);
      void reset();

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_PROBE_AVG_MAG_SQRD_C_IMPL_H */
