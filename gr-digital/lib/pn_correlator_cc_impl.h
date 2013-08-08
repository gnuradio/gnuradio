/* -*- c++ -*- */
/*
 * Copyright 2007,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_PN_CORRELATOR_CC_IMPL_H
#define INCLUDED_GR_PN_CORRELATOR_CC_IMPL_H

#include <gnuradio/digital/pn_correlator_cc.h>
#include <gnuradio/digital/glfsr.h>

namespace gr {
  namespace digital {

    class pn_correlator_cc_impl : public pn_correlator_cc
    {
    private:
      int    d_len;
      float  d_pn;
      glfsr *d_reference;

    public:
      pn_correlator_cc_impl(int degree, int mask=0, int seed=1);
      ~pn_correlator_cc_impl();
      
      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_GR_PN_CORRELATOR_CC_IMPL_H */
