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

/* @WARNING@ */

#ifndef @GUARD_NAME@
#define	@GUARD_NAME@

#include <gnuradio/filter/api.h>
#include <gnuradio/filter/fir_filter.h>
#include <gnuradio/filter/@BASE_NAME@.h>
#include <vector>

namespace gr {
  namespace filter {

    class FILTER_API @IMPL_NAME@ : public @BASE_NAME@
    {
    private:
      bool d_updated;
      std::vector<kernel::@FIR_TYPE@ *> d_firs;
      std::vector<@TAP_TYPE@> d_new_taps;

      void install_taps(const std::vector<@TAP_TYPE@> &taps);

    public:
      @IMPL_NAME@(unsigned interpolation,
		  const std::vector<@TAP_TYPE@> &taps);

      ~@IMPL_NAME@();

      void set_taps(const std::vector<@TAP_TYPE@> &taps);
      std::vector<@TAP_TYPE@> taps() const;

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace filter */
} /* namespace gr */

#endif /* @GUARD_NAME@ */
