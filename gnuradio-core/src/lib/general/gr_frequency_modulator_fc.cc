/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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

#include <gr_frequency_modulator_fc.h>
#include <gr_io_signature.h>
#include <gr_sincos.h>
#include <math.h>


gr_frequency_modulator_fc_sptr gr_make_frequency_modulator_fc (double sensitivity)
{
  return gr_frequency_modulator_fc_sptr (new gr_frequency_modulator_fc (sensitivity));
}

gr_frequency_modulator_fc::gr_frequency_modulator_fc (double sensitivity)
  : gr_sync_block ("frequency_modulator_fc",
		   gr_make_io_signature (1, 1, sizeof (float)),
		   gr_make_io_signature (1, 1, sizeof (gr_complex))),
    d_sensitivity (sensitivity), d_phase (0)
{
}

int
gr_frequency_modulator_fc::work (int noutput_items,
				 gr_vector_const_void_star &input_items,
				 gr_vector_void_star &output_items)
{
  const float *in = (const float *) input_items[0];
  gr_complex *out = (gr_complex *) output_items[0];

  for (int i = 0; i < noutput_items; i++){
    d_phase = d_phase + d_sensitivity * in[i];
    float oi, oq;
    gr_sincosf (d_phase, &oq, &oi);
    out[i] = gr_complex (oi, oq);
  }

  // Limit the phase accumulator to [-16*pi,16*pi]
  // to avoid loss of precision in the addition above.

  if (fabs (d_phase) > 16 * M_PI){
    double ii = trunc (d_phase / (2 * M_PI));
    d_phase = d_phase - (ii * 2 * M_PI);
  }

  return noutput_items;
}
