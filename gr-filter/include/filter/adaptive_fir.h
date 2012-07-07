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

#ifndef INCLUDED_FILTER_ADAPTIVE_FIR_H
#define	INCLUDED_FILTER_ADAPTIVE_FIR_H

#include <filter/api.h>
#include <filter/fir_filter.h>

namespace gr {
  namespace filter {
    namespace kernel {

      /*!
       * \brief Adaptive FIR filter kernel with gr_complex input,
       * gr_complex output and gr_complex taps
       *
       * This class implements an adaptive FIR filter. Any class
       * actually wanting to use adaptive FIRs will contain an object of
       * this class.
       */
      class FILTER_API adaptive_fir_ccc : public fir_filter_ccc
      {
      public:
	adaptive_fir_ccc(int decimation,
			 const std::vector<gr_complex> &taps);
	~adaptive_fir_ccc();

      protected:
	// Override to calculate error signal per output
	virtual gr_complex error(const gr_complex &out) = 0;

	// Override to calculate new weight from old, corresponding input
	virtual void update_tap(gr_complex &tap, const gr_complex &in) = 0;

	gr_complex d_error;
      };


      /*!
       * \brief Adaptive FIR filter kernel with gr_complex input,
       * gr_complex output and float taps
       *
       * This class implements an adaptive FIR filter. Any class
       * actually wanting to use adaptive FIRs will contain an object of
       * this class.
       */
      class FILTER_API adaptive_fir_ccf : public fir_filter_ccf
      {
      public:
	adaptive_fir_ccf(int decimation,
			 const std::vector<float> &taps);
	~adaptive_fir_ccf();

      protected:
	// Override to calculate error signal per output
	virtual float error(const gr_complex &out) = 0;

	// Override to calculate new weight from old, corresponding input
	virtual void update_tap(float &tap, const gr_complex &in) = 0;

	float d_error;
      };

    } /* namespace kernel */
  } /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_FILTER_ADAPTIVE_FIR_H */
