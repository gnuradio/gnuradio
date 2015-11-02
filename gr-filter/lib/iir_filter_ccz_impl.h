/* -*- c++ -*- */
/*
 * Copyright 2004,2012,2014 Free Software Foundation, Inc.
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

#ifndef INCLUDED_IIR_FILTER_CCZ_IMPL_H
#define	INCLUDED_IIR_FILTER_CCZ_IMPL_H

#include <gnuradio/filter/iir_filter.h>
#include <gnuradio/filter/iir_filter_ccz.h>

namespace gr {
  namespace filter {

    class FILTER_API iir_filter_ccz_impl : public iir_filter_ccz
    {
    private:
      bool d_updated;
      kernel::iir_filter<gr_complex, gr_complex, gr_complexd, gr_complexd> *d_iir;
      std::vector<gr_complexd> d_new_fftaps;
      std::vector<gr_complexd> d_new_fbtaps;

    public:
      iir_filter_ccz_impl(const std::vector<gr_complexd> &fftaps,
			  const std::vector<gr_complexd> &fbtaps,
			  bool oldstyle=true);
      ~iir_filter_ccz_impl();

      void set_taps(const std::vector<gr_complexd> &fftaps,
		    const std::vector<gr_complexd> &fbtaps);

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_IIR_FILTER_CCZ_IMPL_H */
