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

#include <gnuradio/atsc/equalizer_lms2_impl.h>
#include <assert.h>
#include <algorithm>
#include <gnuradio/atsc/pnXXX_impl.h>
#include <cmath>
#include <stdlib.h>
#include <gnuradio/math.h>
#include <stdio.h>
#include <boost/math/special_functions/fpclassify.hpp>

using std::min;
using std::max;

static const int	NFFTAPS =  64;
static const int	NFBTAPS = 192;

// the length of the field sync pattern that we know unequivocally
static const int KNOWN_FIELD_SYNC_LENGTH = 4 + 511 + 3 * 63;

// static const float *get_data_seg_sync_training_sequence (int offset);
static int          get_field_sync_training_sequence_length (int offset);
static const float *get_field_sync_training_sequence (int which_field, int offset);

static inline int
wrap (int d)
{
  assert (d >= 0 && d <= (2 * NFBTAPS));

  if(d >= NFBTAPS)
    return d - NFBTAPS;
  return d;
}

static inline float
slice (float d)
{
  if (boost::math::isnan (d))
    return 0.0;

  if (d >= 0.0){
    if (d >= 4.0){
      if (d >= 6.0)
	return 7.0;
      else
	return 5.0;
    }
    if (d >= 2.0)
      return 3.0;
    return 1.0;
  }
  else
    return -slice (-d);
}

atsci_equalizer_lms2::atsci_equalizer_lms2 ()
  : d_taps_ff (NFFTAPS), d_taps_fb (NFBTAPS), d_old_output (NFBTAPS)
{
  for (int i = 0; i < NFFTAPS; i++) {
    d_taps_ff[i] = 0.0;
  }
  for (int i = 0; i < NFBTAPS; i++) {
    d_taps_fb[i] = 0.0;
    d_old_output[i] = 0.0;
  }
  d_output_ptr = 0;
  trainingfile=fopen("taps.txt","w");
}

atsci_equalizer_lms2::~atsci_equalizer_lms2 ()
{
  fclose(trainingfile);
}

void
atsci_equalizer_lms2::reset ()
{
  atsci_equalizer::reset ();		// invoke superclass
  for (int i = 0; i < NFFTAPS; i++) {
    d_taps_ff[i] = 0.0;
  }
  for (int i = 0; i < NFBTAPS; i++) {
    d_taps_fb[i] = 0.0;
    d_old_output[i] = 0.0;
  }
  d_output_ptr = 0;
}


int
atsci_equalizer_lms2::ntaps () const
{
  return NFFTAPS + NFBTAPS;
}

int
atsci_equalizer_lms2::npretaps () const
{
  return NFFTAPS;
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
atsci_equalizer_lms2::filter_normal (const float *input_samples,
				   float *output_samples,
				   int   nsamples)
{
  // handle data
  filterN (input_samples, output_samples, nsamples);
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
atsci_equalizer_lms2::filter_data_seg_sync (const float *input_samples,
					  float *output_samples,
					  int   nsamples,
					  int   offset)
{
  // handle data
  //  adaptN (input_samples, get_data_seg_sync_training_sequence (offset),
  //	  output_samples, nsamples);
 filterN (input_samples, output_samples, nsamples);

 //  cerr << "Seg Sync: offset " << offset << "\tnsamples\t" << nsamples << "\t pre, 5 -5 -5 5\t" <<
 // output_samples[0] << "\t" << output_samples[1] << "\t" << output_samples[2] << "\t" << output_samples[3] << endl;

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
atsci_equalizer_lms2::filter_field_sync (const float *input_samples,
				       float *output_samples,
				       int   nsamples,
				       int   offset,
				       int   which_field)
{
  // Only the first 4 + 511 + 3 * 63 symbols are completely defined.
  // Those after that the symbols are bilevel, so we could use decision feedback and use
  // that to train, but for now, don't train on them.

  int	n = min (nsamples, get_field_sync_training_sequence_length (offset));

  // handle known training sequence
  adaptN (input_samples, get_field_sync_training_sequence (which_field, offset),
		    output_samples, n);

  // just filter any unknown portion
  if (nsamples > n)
    filterN (&input_samples[n], &output_samples[n], nsamples - n);

  if (offset == 0 && nsamples > 0){
    for (int i = 0; i < NFFTAPS; i++)
      fprintf(trainingfile,"%f ",d_taps_ff[i]);
    for (int i = 0; i < NFBTAPS; i++)
      fprintf(trainingfile,"%f ",d_taps_fb[i]);
    fprintf (trainingfile,"\n");
  }

}

// ----------------------------------------------------------------

//
// filter a single output
//
float
atsci_equalizer_lms2::filter1 (const float input[])
{
  static const int N_UNROLL = 4;

  float	acc0 = 0;
  float	acc1 = 0;
  float	acc2 = 0;
  float	acc3 = 0;
  float acc = 0;


  unsigned	i = 0;
  unsigned	n = (NFFTAPS / N_UNROLL) * N_UNROLL;

  for (i = 0; i < n; i += N_UNROLL){
    acc0 += d_taps_ff[i + 0] * input[i + 0];
    acc1 += d_taps_ff[i + 1] * input[i + 1];
    acc2 += d_taps_ff[i + 2] * input[i + 2];
    acc3 += d_taps_ff[i + 3] * input[i + 3];
  }

  for (; i < (unsigned) NFFTAPS; i++)
    acc0 += d_taps_ff[i] * input[i];

  acc = (acc0 + acc1 + acc2 + acc3);

  d_output_ptr = wrap (d_output_ptr + 1);

  for (int i = 0; i < NFBTAPS; i++) {
    acc -= d_taps_fb[i] * d_old_output[wrap(i + d_output_ptr)];
  }

  if (boost::math::isnan (acc)){
    abort ();
  }

  d_old_output[d_output_ptr] = slice (acc);
  return acc;
}

//
// filter and adapt a single output
//
float kludge ()
{
  return 0.0;
}

float
atsci_equalizer_lms2::adapt1 (const float input[], float ideal_output)
{
  static const double BETA = 0.00005;	// FIXME figure out what this ought to be
					// FIXME add gear-shifting

  double y = filter1 (input);
  double e = y - ideal_output;

  // update taps...
  for (int i = 0; i < NFFTAPS; i++){
    d_taps_ff[i] = d_taps_ff[i] - BETA * e * (double)(input[i]);
  }

  for (int i = 0; i < NFBTAPS; i++){
    // d_taps_fb[i] = d_taps_fb[i] - BETA * e * (double)d_old_output[wrap(i+d_output_ptr)];
    d_taps_fb[i] = d_taps_fb[i] - kludge() * e * (double)d_old_output[wrap(i+d_output_ptr)];
  }

  return y;
}

void
atsci_equalizer_lms2::filterN (const float *input_samples,
			     float *output_samples,
			     int nsamples)
{
  for (int i = 0; i < nsamples; i++)
    output_samples[i] = filter1 (&input_samples[i]);
}


void
atsci_equalizer_lms2::adaptN (const float *input_samples,
			    const float *training_pattern,
			    float *output_samples,
			    int    nsamples)
{
  for (int i = 0; i < nsamples; i++)
    output_samples[i] = adapt1 (&input_samples[i], training_pattern[i]);
}

// ----------------------------------------------------------------

static float
bin_map (int bit)
{
  return bit ? +5 : -5;
}

static void
init_field_sync_common (float *p, int mask)

{
  int  i = 0;

  p[i++] = bin_map (1);			// data segment sync pulse
  p[i++] = bin_map (0);
  p[i++] = bin_map (0);
  p[i++] = bin_map (1);

  for (int j = 0; j < 511; j++)		// PN511
    p[i++] = bin_map (atsc_pn511[j]);

  for (int j = 0; j < 63; j++)		// PN63
    p[i++] = bin_map (atsc_pn63[j]);

  for (int j = 0; j < 63; j++)		// PN63, toggled on field 2
    p[i++] = bin_map (atsc_pn63[j] ^ mask);

  for (int j = 0; j < 63; j++)		// PN63
    p[i++] = bin_map (atsc_pn63[j]);

  assert (i == KNOWN_FIELD_SYNC_LENGTH);
}

#if 0
static const float *
get_data_seg_sync_training_sequence (int offset)
{
  static const float training_data[4] = { +5, -5, -5, +5 };
  return &training_data[offset];
}
#endif

static int
get_field_sync_training_sequence_length (int offset)
{
  return max (0, KNOWN_FIELD_SYNC_LENGTH - offset);
}

static const float *
get_field_sync_training_sequence (int which_field, int offset)
{
  static float *field_1 = 0;
  static float *field_2 = 0;

  if (field_1 == 0){
    field_1 = new float[KNOWN_FIELD_SYNC_LENGTH];
    field_2 = new float[KNOWN_FIELD_SYNC_LENGTH];
    init_field_sync_common (field_1, 0);
    init_field_sync_common (field_2, 1);
  }

  if (which_field == 0)
    return &field_1[offset];
  else
    return &field_2[offset];
}
