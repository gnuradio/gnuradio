/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
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

#ifndef INCLUDED_ATSC_BIT_TIMING_LOOP_H
#define INCLUDED_ATSC_BIT_TIMING_LOOP_H

#include <gnuradio/atsc/api.h>
#include <cstdio>
#include <gnuradio/block.h>
#include <gnuradio/filter/mmse_fir_interpolator_ff.h>
#include <gnuradio/filter/single_pole_iir.h>
#include <gnuradio/atsc/consts.h>
#include <gnuradio/atsc/syminfo_impl.h>

class atsc_bit_timing_loop;
typedef boost::shared_ptr<atsc_bit_timing_loop> atsc_bit_timing_loop_sptr;

ATSC_API atsc_bit_timing_loop_sptr atsc_make_bit_timing_loop( float input_rate );

/*!
 * \brief ATSC BitTimingLoop3
 * \ingroup atsc
 *
 * This class accepts a single real input and produces two outputs,
 *  the raw symbol (float) and the tag (atsc_syminfo)
 */

class ATSC_API atsc_bit_timing_loop : public gr::block
{
	friend ATSC_API atsc_bit_timing_loop_sptr atsc_make_bit_timing_loop( float input_rate );

	atsc_bit_timing_loop( float input_rate );

public:
	void reset();

	~atsc_bit_timing_loop () { };

	void forecast (int noutput_items, gr_vector_int &ninput_items_required);

	int  general_work (int noutput_items,
				gr_vector_int &ninput_items,
				gr_vector_const_void_star &input_items,
				gr_vector_void_star &output_items);

protected:

	static const double	LOOP_FILTER_TAP = 0.0005;	// 0.0005 works
	static const double	ADJUSTMENT_GAIN = 1.0e-5 / (10 * ATSC_DATA_SEGMENT_LENGTH);
	static const int	SYMBOL_INDEX_OFFSET = 3;
	static const int	MIN_SEG_LOCK_CORRELATION_VALUE = 5;
	static const int	SSI_MIN = -16;
	static const int	SSI_MAX =  15;

	gr::filter::single_pole_iir<float,float,float> d_loop; // ``VCO'' loop filter
	gr::filter::mmse_fir_interpolator_ff    d_interp;
	unsigned long long                      d_next_input;
	double                  d_rx_clock_to_symbol_freq;
	int                     d_si;
	double                  d_w;	// ratio of PERIOD of Tx to Rx clocks
	double                  d_mu;	// fractional delay [0,1]
	int                     d_incr;

	float			sample_mem[ATSC_DATA_SEGMENT_LENGTH];

	double                  d_timing_adjust;
	int                     d_counter;	// free running mod 832 counter
	int                     d_symbol_index;
	bool                    d_seg_locked;

	int                     d_sr;	// 4 bit shift register

	signed char             d_integrator[ATSC_DATA_SEGMENT_LENGTH];

	bool incr_counter ()
	{
		d_counter++;
		if (d_counter >= ATSC_DATA_SEGMENT_LENGTH)
		{
			d_counter = 0;
			return true;
		}
		return false;
	}
};

#endif /* INCLUDED_ATSC_BIT_TIMING_LOOP_H */



