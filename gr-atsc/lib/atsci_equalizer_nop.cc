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

#include <gnuradio/atsc/equalizer_nop_impl.h>
#include <gnuradio/atsc/sync_tag_impl.h>
#include <assert.h>

atsci_equalizer_nop::atsci_equalizer_nop ()
{
}

atsci_equalizer_nop::~atsci_equalizer_nop ()
{
}

void
atsci_equalizer_nop::reset ()
{
  atsci_equalizer::reset ();	// invoke superclass
}

int
atsci_equalizer_nop::ntaps () const
{
  return 1;
}

int
atsci_equalizer_nop::npretaps () const
{
  return 0;
}

/*!
 * Input range is known NOT TO CONTAIN data segment syncs
 * or field syncs.  This should be the fast path.  In the
 * non decicion directed case, this just runs the input
 * through the filter without adapting it.
 *
 * \p input_samples has (nsamples + ntaps() - 1) valid entries.
 * input_samples[0] .. input_samples[nsamples - 1 + ntaps() - 1] may be
 * referenced to compute the output values.
 */
void
atsci_equalizer_nop::filter_normal (const float *input_samples,
				   float *output_samples,
				   int   nsamples)
{
  for (int i = 0; i < nsamples; i++){
    output_samples[i] = scale (input_samples[i]);
  }
}


/*!
 * Input range is known to consist of only a data segment sync or a
 * portion of a data segment sync.  \p nsamples will be in [1,4].
 * \p offset will be in [0,3].  \p offset is the offset of the input
 * from the beginning of the data segment sync pattern.
 *
 * \p input_samples has (nsamples + ntaps() - 1) valid entries.
 * input_samples[0] .. input_samples[nsamples - 1 + ntaps() - 1] may be
 * referenced to compute the output values.
 */
void
atsci_equalizer_nop::filter_data_seg_sync (const float *input_samples,
					  float *output_samples,
					  int   nsamples,
					  int   offset)
{
  for (int i = 0; i < nsamples; i++){
    output_samples[i] = scale_and_train (input_samples[i]);
  }
}


/*!
 * Input range is known to consist of only a field sync segment or a
 * portion of a field sync segment.  \p nsamples will be in [1,832].
 * \p offset will be in [0,831].  \p offset is the offset of the input
 * from the beginning of the data segment sync pattern.  We consider the
 * 4 symbols of the immediately preceding data segment sync to be the
 * first symbols of the field sync segment.  \p which_field is in [0,1]
 * and specifies which field (duh).
 *
 * \p input_samples has (nsamples + ntaps() - 1) valid entries.
 * input_samples[0] .. input_samples[nsamples - 1 + ntaps() - 1] may be
 * referenced to compute the output values.
 */
void
atsci_equalizer_nop::filter_field_sync (const float *input_samples,
				       float *output_samples,
				       int   nsamples,
				       int   offset,
				       int   which_field)
{
  int	i = 0;

  if (offset == 0 && nsamples > 0){
    output_samples[0] = scale_and_train (input_samples[0]);
    i++;
  }

  for (; i < nsamples; i++){
    output_samples[i] = scale_and_train (input_samples[i]);
  }
}


float
atsci_equalizer_nop::scale_and_train (float input)
{
  return input;
}
