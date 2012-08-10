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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <filter/adaptive_fir.h>
#include <gr_io_signature.h>

namespace gr {
  namespace filter {
    namespace kernel {

      adaptive_fir_ccc::adaptive_fir_ccc(int decimation,
					 const std::vector<gr_complex> &taps)
	: fir_filter_ccc(decimation, taps)
      {
      }

      adaptive_fir_ccc::~adaptive_fir_ccc()
      {
      }


      /**************************************************************/


      adaptive_fir_ccf::adaptive_fir_ccf(int decimation,
					 const std::vector<float> &taps)
	: fir_filter_ccf(decimation, taps)
      {
      }

      adaptive_fir_ccf::~adaptive_fir_ccf()
      {
      }

    } /* namespace kernel */
  } /* namespace filter */
} /* namespace gr */
