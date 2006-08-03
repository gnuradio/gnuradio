/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

// @WARNING@

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <@NAME@.h>
#include <algorithm>
#include <gr_io_signature.h>
#include <stdexcept>
#include <gr_complex.h>


@NAME@::@NAME@ (double sampling_freq, gr_waveform_t waveform,
		double frequency, double ampl, @TYPE@ offset)
  : gr_sync_block ("@BASE_NAME@",
		   gr_make_io_signature (0, 0, 0),
		   gr_make_io_signature (1, 1, sizeof (@TYPE@))),
    d_sampling_freq (sampling_freq), d_waveform (waveform), d_frequency (frequency),
    d_ampl (ampl), d_offset (offset)
{
  d_nco.set_freq (2 * M_PI * d_frequency / d_sampling_freq);
}

@NAME@_sptr
gr_make_@BASE_NAME@ (double sampling_freq, gr_waveform_t waveform,
		     double frequency, double ampl, @TYPE@ offset)
{
  return @NAME@_sptr (new @NAME@ (sampling_freq, waveform, frequency, ampl, offset));
}

int
@NAME@::work (int noutput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items)
{
  @TYPE@ *optr = (@TYPE@ *) output_items[0];
  @TYPE@ t;

  switch (d_waveform){

#if @IS_COMPLEX@	// complex?

  case GR_CONST_WAVE:
    t = (gr_complex) d_ampl + d_offset;
    for (int i = 0; i < noutput_items; i++)	// FIXME unroll
      optr[i] = t;
    break;
    
  case GR_SIN_WAVE:
  case GR_COS_WAVE:
    d_nco.sincos (optr, noutput_items, d_ampl);
    if (d_offset == gr_complex(0,0))
      break;

    for (int i = 0; i < noutput_items; i++){
      optr[i] += d_offset;
    }
    break;

#else			// nope...

  case GR_CONST_WAVE:
    t = (@TYPE@) d_ampl + d_offset;
    for (int i = 0; i < noutput_items; i++)	// FIXME unroll
      optr[i] = t;
    break;
    
  case GR_SIN_WAVE:
    d_nco.sin (optr, noutput_items, d_ampl);
    if (d_offset == 0)
      break;

    for (int i = 0; i < noutput_items; i++){
      optr[i] += d_offset;
    }
    break;

  case GR_COS_WAVE:
    d_nco.cos (optr, noutput_items, d_ampl);
    if (d_offset == 0)
      break;

    for (int i = 0; i < noutput_items; i++){
      optr[i] += d_offset;
    }
    break;
#endif

  default:
    throw std::runtime_error ("gr_sig_source: invalid waveform");
  }

  return noutput_items;
}

void
@NAME@::set_sampling_freq (double sampling_freq)
{
  d_sampling_freq = sampling_freq;
  d_nco.set_freq (2 * M_PI * d_frequency / d_sampling_freq);
}

void
@NAME@::set_waveform (gr_waveform_t waveform)
{
  d_waveform = waveform;
}

void
@NAME@::set_frequency (double frequency)
{
  d_frequency = frequency;
  d_nco.set_freq (2 * M_PI * d_frequency / d_sampling_freq);
}

void
@NAME@::set_amplitude (double ampl)
{
  d_ampl = ampl;
}

void
@NAME@::set_offset (@TYPE@ offset)
{
  d_offset = offset;
}

