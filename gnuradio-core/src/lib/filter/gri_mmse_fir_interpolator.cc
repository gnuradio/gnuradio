/* -*- c++ -*- */
/*
 * Copyright 2002 Free Software Foundation, Inc.
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
#include <config.h>
#endif
#include <gri_mmse_fir_interpolator.h>
#include <gr_fir_util.h>
#include <gr_fir_fff.h>
#include <assert.h>
#include <cmath>
#include "interpolator_taps.h"

gri_mmse_fir_interpolator::gri_mmse_fir_interpolator ()
{
  filters.resize (NSTEPS + 1);
  
  for (int i = 0; i < NSTEPS + 1; i++){
    std::vector<float> t (&taps[i][0], &taps[i][NTAPS]);
    filters[i] = gr_fir_util::create_gr_fir_fff (t);
  }
}

gri_mmse_fir_interpolator::~gri_mmse_fir_interpolator ()
{
  for (int i = 0; i < NSTEPS + 1; i++)
    delete filters[i];
}

unsigned
gri_mmse_fir_interpolator::ntaps () const
{
  return NTAPS;
}

unsigned
gri_mmse_fir_interpolator::nsteps () const
{
  return NSTEPS;
}

float
gri_mmse_fir_interpolator::interpolate (const float input[], float mu) const
{
  int	imu = (int) rint (mu * NSTEPS);

  assert (imu >= 0);
  assert (imu <= NSTEPS);

  float r = filters[imu]->filter (input);
  return r;
}
