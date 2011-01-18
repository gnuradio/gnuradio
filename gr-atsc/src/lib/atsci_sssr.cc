/* -*- c++ -*- */
/*
 * Copyright 2002,2008 Free Software Foundation, Inc.
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

#include <atsci_sssr.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <assert.h>
#include <atsci_diag_output.h>
#include <gr_math.h>
#include <stdio.h>
#include <boost/math/special_functions/sign.hpp>

/*
 * ----------------------------------------------------------------
 * Segment Integrator Pre-Processor
 *
 * Compute weight passed to integrator based on
 * correlation result and ncorr_gain2
 */

inline static int
sipp (bool digcorr_output)
{
  if (digcorr_output)
    return +2;			// positive correlation
  else
    return -1;			// no correlation
}

/* 
 * ----------------------------------------------------------------
 * Segment Sync Integrator...
 */

static const int SSI_MIN = -16;
static const int SSI_MAX =  15;

void
sssr::seg_sync_integrator::reset ()
{
  for (int i = 0; i < ATSC_DATA_SEGMENT_LENGTH; i++)
    d_integrator[i] = SSI_MIN;
}
  
int
sssr::seg_sync_integrator::update (int weight, int index)
{
  int	t = d_integrator[index] + weight;
  t = std::max (t, SSI_MIN);
  t = std::min (t, SSI_MAX);
  d_integrator[index] = t;

  return t;
}

int
sssr::seg_sync_integrator::find_max (int *v)
{
  int	best_value = d_integrator[0];
  int	best_index = 0;
  
  for (int i = 1; i < ATSC_DATA_SEGMENT_LENGTH; i++)
    if (d_integrator[i] > best_value){
      best_value = d_integrator[i];
      best_index = i;
    }

  *v = best_value;
  return best_index;
}

/*
 * ----------------------------------------------------------------
 * Segment Sync and Symbol recovery
 */

static const int	SYMBOL_INDEX_OFFSET = 3;
static const int	MIN_SEG_LOCK_CORRELATION_VALUE = 5;

atsci_sssr::atsci_sssr ()
  : d_debug_fp(0)
{
  reset ();
  
  if (_SSSR_DIAG_OUTPUT_){
    const char *file = "sssr.ssout";
    if ((d_debug_fp = fopen (file, "wb")) == 0){
      perror (file);
      exit (1);
    }
  }
}

atsci_sssr::~atsci_sssr ()
{
  if (d_debug_fp)
    fclose (d_debug_fp);
}

void
atsci_sssr::reset ()
{
  d_correlator.reset ();
  d_integrator.reset ();
  d_quad_filter.reset ();

  for (int i = 0; i < ATSC_DATA_SEGMENT_LENGTH; i++)
    d_quad_output[i] = 0;
  
  d_timing_adjust = 0;
  d_counter = 0;
  d_symbol_index = 0;
  d_seg_locked = false;
}

void
atsci_sssr::update (sssr::sample_t sample_in,	 // input
		   bool		 *seg_locked,	 // are we seeing segment syncs?
		   int		 *symbol_index,	 // 0..831
		   double	 *timing_adjust) // how much to adjust timing
{
  double qo = d_quad_filter.update (sample_in);
  d_quad_output[d_counter] = qo;

  int bit = boost::math::signbit (sample_in) ^ 1;	// slice on sign: + => 1, - => 0
  int corr_out = d_correlator.update (bit);
  int weight = sipp (corr_out);
  int corr_value = d_integrator.update (weight, d_counter);
  int best_correlation_index = -1;

  incr_symbol_index ();
  if (incr_counter ()){		// counter just wrapped...
    int	best_correlation_value;
    best_correlation_index = d_integrator.find_max (&best_correlation_value);
    d_seg_locked = best_correlation_value >= MIN_SEG_LOCK_CORRELATION_VALUE;
    d_timing_adjust = d_quad_output[best_correlation_index];

    d_symbol_index = SYMBOL_INDEX_OFFSET - 1 - best_correlation_index;
    if (d_symbol_index < 0)
      d_symbol_index += ATSC_DATA_SEGMENT_LENGTH;
  }

  *seg_locked = d_seg_locked;
  *symbol_index = d_symbol_index;
  *timing_adjust = d_timing_adjust;

  if (_SSSR_DIAG_OUTPUT_){
    float	iodata[7];
    iodata[0] = bit;
    iodata[1] = corr_value;
    iodata[2] = qo;
    iodata[3] = d_counter;
    iodata[4] = d_symbol_index;
    iodata[5] = best_correlation_index;
    iodata[6] = d_timing_adjust;

    if (fwrite (iodata, sizeof (iodata), 1, d_debug_fp) != 1){
      perror ("fwrite: sssr");
      exit (1);
    }
  }

}

/*
 * ----------------------------------------------------------------
 * Interpolator control for Seg & Symbol Sync Recovery
 */

static const double	LOOP_FILTER_TAP = 0.00025;	// 0.0005 works
static const double	ADJUSTMENT_GAIN = 1.0e-5 / (10 * ATSC_DATA_SEGMENT_LENGTH);
  
atsci_interpolator::atsci_interpolator (double nominal_ratio_of_rx_clock_to_symbol_freq)
  : d_debug_fp(0)
{
// Tweaked ratio from 1.8 to 1.78 to support input rate of 19.2MHz
  assert (nominal_ratio_of_rx_clock_to_symbol_freq >= 1.78);
  d_nominal_ratio_of_rx_clock_to_symbol_freq =
    nominal_ratio_of_rx_clock_to_symbol_freq;

  d_loop.set_taps (LOOP_FILTER_TAP);

  reset ();

  if (_SSSR_DIAG_OUTPUT_){
    const char *file = "interp.ssout";
    if ((d_debug_fp = fopen (file, "wb")) == 0){
      perror (file);
      exit (1);
    }
  }

}

atsci_interpolator::~atsci_interpolator ()
{
  if (d_debug_fp)
    fclose (d_debug_fp);
}

void
atsci_interpolator::reset ()
{
  d_w = d_nominal_ratio_of_rx_clock_to_symbol_freq;
  d_mu = 0.5;
  d_incr = 0;
  d_loop.reset ();
}

bool
atsci_interpolator::update (
	   const sssr::sample_t input_samples[],	// I: vector of samples
	   int	     		nsamples,		// I: total number of samples
	   int	    	       *index,			// I/O: current input index
	   double     		timing_adjustment,	// I: how much to bump timing
	   sssr::sample_t      *output_sample)
{
  if (*index + (int) ntaps () > nsamples)
    return false;

  // FIXME Confirm that this is right.  I think it is.  It was (1-d_mu)
  *output_sample = d_interp.interpolate (&input_samples[*index], d_mu);  

  double filter_out = 0;
  
#if 0

  filter_out = d_loop.filter (timing_adjustment);
  d_w = d_w + ADJUSTMENT_GAIN * filter_out * 1e-3;

#elif 1

  filter_out = d_loop.filter (timing_adjustment);
  d_mu = d_mu + ADJUSTMENT_GAIN * 10e3 * filter_out;

#else

  static const double alpha = 0.01;
  static const double beta = alpha * alpha / 16;

  double x = ADJUSTMENT_GAIN * 10e3 * timing_adjustment;

  d_mu = d_mu + alpha * x;	// conceptually "phase"
  d_w  = d_w  + beta * x;	// conceptually "frequency"
  
#endif
  
  double s = d_mu + d_w;
  double float_incr = floor (s);
  d_mu = s - float_incr;
  d_incr = (int) float_incr;

  assert (d_incr >= 1 && d_incr <= 3);
  *index += d_incr;

  if (_SSSR_DIAG_OUTPUT_){
    float	iodata[6];
    iodata[0] = timing_adjustment;
    iodata[1] = filter_out;
    iodata[2] = d_w;
    iodata[3] = d_mu;
    iodata[4] = d_incr;
    iodata[5] = *output_sample;
    if (fwrite (iodata, sizeof (iodata), 1, d_debug_fp) != 1){
      perror ("fwrite: interpolate");
      exit (1);
    }
  }

  return true;
}
