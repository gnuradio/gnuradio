/* -*- c++ -*- */
/*
 * Copyright 2004-2006,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_SINGLE_POLE_IIR_FILTER_FF_IMPL_H
#define	INCLUDED_SINGLE_POLE_IIR_FILTER_FF_IMPL_H

#include <gnuradio/filter/single_pole_iir.h>
#include <gnuradio/filter/single_pole_iir_filter_ff.h>
#include <gnuradio/sync_block.h>
#include <stdexcept>

namespace gr {
  namespace filter {

    class FILTER_API single_pole_iir_filter_ff_impl : public single_pole_iir_filter_ff
    {
    private:
      unsigned int d_vlen;
      std::vector<single_pole_iir<float,float,double> > d_iir;

    public:
      single_pole_iir_filter_ff_impl(double alpha, unsigned int vlen);
      ~single_pole_iir_filter_ff_impl();

      void set_taps(double alpha);

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_SINGLE_POLE_IIR_FILTER_FF_IMPL_H */
