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

#ifndef INCLUDED_FILTER_ADAPTIVE_FIR_CCC_IMPL_H
#define	INCLUDED_FILTER_ADAPTIVE_FIR_CCC_IMPL_H

#include <filter/adaptive_fir_ccc.h>
#include <filter/adaptive_fir.h>
#include <gr_types.h>

namespace gr {
  namespace filter {

    class FILTER_API adaptive_fir_ccc_impl :
      public adaptive_fir_ccc, public kernel::adaptive_fir_ccc
    {
    private:
      std::vector<gr_complex> d_new_taps;
      bool d_updated;
      
    protected:
      // Override to calculate error signal per output
      gr_complex error(const gr_complex &out);

      // Override to calculate new weight from old, corresponding input
      void update_tap(gr_complex &tap, const gr_complex &in);

    public:
      void set_taps(const std::vector<gr_complex> &taps);
      std::vector<gr_complex> taps() const;

      adaptive_fir_ccc_impl(const char *name, int decimation,
			    const std::vector<gr_complex> &taps);

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_FILTER_ADAPTIVE_FIR_CCC_IMPL_H */
