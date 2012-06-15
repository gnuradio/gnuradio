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

#ifndef INCLUDED_FILTER_ADAPTIVE_FIR_CCF_H
#define	INCLUDED_FILTER_ADAPTIVE_FIR_CCF_H

#include <filter/api.h>
#include <gr_sync_decimator.h>

namespace gr {
  namespace filter {

    class FILTER_API adaptive_fir_ccf : virtual public gr_sync_decimator
    {
    public:
      // gr::filter::adaptive_fir_ccf::sptr
      typedef boost::shared_ptr<adaptive_fir_ccf> sptr;

      /*!
       * \brief Adaptive FIR filter with gr_complex input, gr_complex output and float taps
       * \ingroup filter_blk
       */
      static FILTER_API sptr make(const char *name, int decimation,
				  const std::vector<float> &taps);

      virtual void set_taps(const std::vector<float> &taps) = 0;
      virtual std::vector<float> taps() = 0;
    };

  } /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_FILTER_ADAPTIVE_FIR_CCF_H */
