/* -*- c++ -*- */
/*
 * Copyright 2008,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_PROBE_DENSITY_B_IMPL_H
#define INCLUDED_GR_PROBE_DENSITY_B_IMPL_H

#include <gnuradio/digital/probe_density_b.h>

namespace gr {
  namespace digital {

    class probe_density_b_impl : public probe_density_b
    {
    private:
      double d_alpha;
      double d_beta;
      double d_density;

    public:
      probe_density_b_impl(double alpha);
      ~probe_density_b_impl();

      /*!
       * \brief Returns the current density value
       */
      double density() const { return d_density; }

      /*!
       * \brief Set the average filter constant
       */
      void set_alpha(double alpha);

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_GR_PROBE_DENSITY_B_IMPL_H */
