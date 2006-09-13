/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
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
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <code_metrics.h>
#include <iostream>
#include <math.h>
#include <assert.h>

template<typename pdf_fcn_io_t>
code_metrics_table<pdf_fcn_io_t>*
libecc_code_metrics_create_table
(pdf_fcn_io_t (*pdf_fcn_0_bit) (pdf_fcn_io_t),
 pdf_fcn_io_t (*pdf_fcn_1_bit) (pdf_fcn_io_t),
 size_t n_samples,
 pdf_fcn_io_t min_sample,
 pdf_fcn_io_t max_sample,
 int sample_precision)
{
  if (! pdf_fcn_0_bit) {
    std::cerr << "libecc_code_metrics_create_table: Error: "
      "pdf_fcn_0_bit must be a non-null pointer to function.\n";
    assert (0);
  }
  if (! pdf_fcn_1_bit) {
    std::cerr << "libecc_code_metrics_create_table: Error: "
      "pdf_fcn_0_bit must be a non-null pointer to function.\n";
    assert (0);
  }
  if (n_samples < 2) {
    std::cerr << "libecc_code_metrics_create_table: Error: "
      "n_samples must be at least 2.\n";
    assert (0);
  }
  if (min_sample >= max_sample) {
    std::cerr << "libecc_code_metrics_create_table: Error: "
      "min_sample must be less than max_sample.\n";
    assert (0);
  }
  if ((sample_precision < 0) | (sample_precision > 32)) {
    std::cerr << "libecc_code_metrics_create_table: Error: "
      "sample_precision must be between 0 and 32.\n";
    assert (0);
  }

  code_metrics_table<pdf_fcn_io_t>* t_code_metrics_table;

  if (sample_precision == 0) {
    // float
    t_code_metrics_table = new code_metrics_table_work
      <pdf_fcn_io_t, float>(pdf_fcn_0_bit,
			    pdf_fcn_1_bit,
			    n_samples,
			    min_sample,
			    max_sample);
  } else if (sample_precision <= 8) {
    // use char
    t_code_metrics_table = new code_metrics_table_work
      <pdf_fcn_io_t, unsigned char>(pdf_fcn_0_bit,
				    pdf_fcn_1_bit,
				    n_samples,
				    min_sample,
				    max_sample,
				    sample_precision);
  } else if (sample_precision <= 16) {
    // use short
    t_code_metrics_table = new code_metrics_table_work
      <pdf_fcn_io_t, unsigned short>(pdf_fcn_0_bit,
				     pdf_fcn_1_bit,
				     n_samples,
				     min_sample,
				     max_sample,
				     sample_precision);
  } else {
    // use long
    t_code_metrics_table = new code_metrics_table_work
      <pdf_fcn_io_t, unsigned long>(pdf_fcn_0_bit,
				    pdf_fcn_1_bit,
				    n_samples,
				    min_sample,
				    max_sample,
				    sample_precision);
  }

  return (t_code_metrics_table);
}

template<typename pdf_fcn_io_t>
code_metrics_table<pdf_fcn_io_t>::code_metrics_table
(pdf_fcn_t pdf_fcn_0_bit,
 pdf_fcn_t pdf_fcn_1_bit,
 size_t n_samples,
 pdf_fcn_io_t min_sample,
 pdf_fcn_io_t max_sample)
{
  // internally, all samples are taken as pdf_fcn_io_t initially, and
  // only converted to other values by their specific constructors.

  d_n_samples = n_samples;
  d_max_sample = max_sample;
  d_min_sample = min_sample;
  d_delta = (max_sample - min_sample) / ((pdf_fcn_io_t) n_samples);
  d_pdf_fcn_0_bit = pdf_fcn_0_bit;
  d_pdf_fcn_1_bit = pdf_fcn_1_bit;

  // use a sub-sample by 100 to better determine the actual "bin"
  // probability values for each actual sample.  Each "bin" is 100
  // delta's less than the min_sample up to the min_sample+delta; then
  // each delta; then from the max_sample-delta to 100 delta's more
  // than the max sample.  Once normalized, these give a reasonable
  // interpretation of the PDF function.

  pdf_fcn_io_t d_sub_delta = d_delta / ((pdf_fcn_io_t) 100);
  pdf_fcn_io_t d_sub_min_sample = d_min_sample - ((pdf_fcn_io_t) 100)*d_delta;
  pdf_fcn_io_t d_sup_max_sample = d_max_sample + ((pdf_fcn_io_t) 100)*d_delta;

  d_pdf_fcn_0_samples.assign (d_n_samples, 0);
  d_pdf_fcn_1_samples.assign (d_n_samples, 0);

  pdf_fcn_io_t t_val, t_sum_0, t_sum_1, t_max_0, t_max_1, t_min_0, t_min_1;
  t_sum_0 = t_sum_1 = t_max_0 = t_max_1 = t_min_0 = t_min_1 = 0;
  size_t m = 0;
  t_val = d_sub_min_sample;
  for (; m < (d_n_samples - 1); m++) {
    pdf_fcn_io_t t_sample_0 = 1;
    pdf_fcn_io_t t_sample_1 = 1;
    for (; t_val < (d_min_sample+d_delta); t_val += d_sub_delta) {
      t_sample_0 += ((*d_pdf_fcn_0_bit)(t_val));
      t_sample_1 += ((*d_pdf_fcn_1_bit)(t_val));
    }
    d_pdf_fcn_0_samples[m] = t_sample_0;
    d_pdf_fcn_0_samples[m] = t_sample_1;
    t_sum_0 += t_sample_0;
    t_sum_1 += t_sample_1;
    if (m == 0) {
      t_max_0 = t_min_0 = t_sample_0;
      t_max_1 = t_min_1 = t_sample_1;
    } else {
      if (t_max_0 < t_sample_0)
	t_max_0 = t_sample_0;
      else if (t_min_0 > t_sample_0)
	t_min_0 = t_sample_0;
      if (t_max_1 < t_sample_1)
	t_max_1 = t_sample_1;
      else if (t_min_1 > t_sample_1)
	t_min_1 = t_sample_1;
    }
  }

  pdf_fcn_io_t t_sample_0 = 1;
  pdf_fcn_io_t t_sample_1 = 1;
  for (; t_val < d_sup_max_sample; t_val += d_sub_delta) {
    t_sample_0 += ((*d_pdf_fcn_0_bit)(t_val));
    t_sample_1 += ((*d_pdf_fcn_1_bit)(t_val));
  }
  d_pdf_fcn_0_samples[m] = t_sample_0;
  d_pdf_fcn_0_samples[m] = t_sample_1;
  t_sum_0 += t_sample_0;
  t_sum_1 += t_sample_1;
  if (t_max_0 < t_sample_0)
    t_max_0 = t_sample_0;
  else if (t_min_0 > t_sample_0)
    t_min_0 = t_sample_0;
  if (t_max_1 < t_sample_1)
    t_max_1 = t_sample_1;
  else if (t_min_1 > t_sample_1)
    t_min_1 = t_sample_1;

  // normalize to the sum, so that these are "real" probabilities.

  for (m = 0; m < d_n_samples; m++) {
    d_pdf_fcn_0_samples[m] /= t_sum_0;
    d_pdf_fcn_1_samples[m] /= t_sum_1;
  }
  t_max_0 /= t_sum_0;
  t_min_0 /= t_sum_0;
  t_max_1 /= t_sum_1;
  t_min_1 /= t_sum_1;

  // take the logf so that metrics can add

  for (m = 0; m < d_n_samples; m++) {
    d_pdf_fcn_0_samples[m] = logf (d_pdf_fcn_0_samples[m]);
    d_pdf_fcn_1_samples[m] = logf (d_pdf_fcn_1_samples[m]);
  }
  t_max_0 = logf (t_max_0);
  t_min_0 = logf (t_min_0);
  t_max_1 = logf (t_max_1);
  t_min_1 = logf (t_min_1);

  // higher (less negative) log-probabilities mean more likely; lower
  // (more negative) mean less likely.  Want metrics which are 0 when
  // most likely and more positive when less likely.  So subtract the
  // max, then negate and normalize to the min (new max) so that the
  // max value is 1 and the min value is 0.

  for (m = 0; m < d_n_samples; m++) {
    d_pdf_fcn_0_samples[m] = ((d_pdf_fcn_0_samples[m] - t_max_0) /
			      (t_min_0 - t_max_0));
    d_pdf_fcn_1_samples[m] = ((d_pdf_fcn_1_samples[m] - t_max_1) /
			      (t_min_1 - t_max_1));
  }

  // correct the delta to the lookup computations

  d_delta = (max_sample - min_sample) / ((pdf_fcn_io_t)(n_samples-1));
}

template<typename pdf_fcn_io_t, typename metric_t>
code_metrics_table_work<pdf_fcn_io_t,metric_t>::code_metrics_table_work
(pdf_fcn_t pdf_fcn_0_bit,
 pdf_fcn_t pdf_fcn_1_bit,
 size_t n_samples,
 pdf_fcn_io_t min_sample,
 pdf_fcn_io_t max_sample,
 int sample_precision)
  : code_metrics_table<pdf_fcn_io_t>
    (pdf_fcn_0_bit,
     pdf_fcn_1_bit,
     n_samples,
     min_sample,
     max_sample)
{
  code_metrics_table<pdf_fcn_io_t>::d_out_item_size_bytes = sizeof (metric_t);
  code_metrics_table<pdf_fcn_io_t>::d_sample_precision = sample_precision;

  // get the scale factor for converting from float to
  // sample_precision maps: 0 -> 0, 1 -> (2^sample_precision)-1 for
  // integers; there is no need for a mapping for float types, since
  // those are already in [0,1].

  pdf_fcn_io_t t_mult = ((sample_precision == 0) ? 1 :
			 ((pdf_fcn_io_t)((2^sample_precision)-1)));

  // convert the 0 bit metrics from float to integer

  d_metric_table_0_bit.assign (n_samples, 0);
  for (size_t m = 0; m < n_samples; m++) {
    d_metric_table_0_bit[m] =
      (metric_t)((code_metrics_table<pdf_fcn_io_t>::d_pdf_fcn_0_samples[m]) *
		 t_mult);
  }

  // clear the old float sample vectors to free memory

  code_metrics_table<pdf_fcn_io_t>::d_pdf_fcn_0_samples.resize (0);

  // convert the 1 bit metrics from float to integer

  d_metric_table_1_bit.assign (n_samples, 0);
  for (size_t m = 0; m < n_samples; m++) {
    d_metric_table_1_bit[m] =
      (metric_t)((code_metrics_table<pdf_fcn_io_t>::d_pdf_fcn_1_samples[m]) *
		 t_mult);
  }

  // clear the old float sample vectors to free memory

  code_metrics_table<pdf_fcn_io_t>::d_pdf_fcn_1_samples.resize (0);
}

template<typename pdf_fcn_io_t, typename metric_t>
void
code_metrics_table_work<pdf_fcn_io_t,metric_t>::lookup
(pdf_fcn_io_t sym,
 void* bit_0,
 void* bit_1)
{
  metric_ptr_t l_bit_0 = (metric_ptr_t) bit_0;
  metric_ptr_t l_bit_1 = (metric_ptr_t) bit_1;

  if (sym <= code_metrics_table<pdf_fcn_io_t>::d_min_sample) {
    *l_bit_0 = d_metric_table_0_bit[0];
    *l_bit_1 = d_metric_table_1_bit[0];
    return;
  }
  if (sym >= code_metrics_table<pdf_fcn_io_t>::d_max_sample) {
    *l_bit_0 = d_metric_table_0_bit.back ();
    *l_bit_1 = d_metric_table_1_bit.back ();
    return;
  }

  size_t l_ndx = (size_t) round
    ((double)((sym - code_metrics_table<pdf_fcn_io_t>::d_min_sample) /
	      code_metrics_table<pdf_fcn_io_t>::d_delta));
  *l_bit_0 = d_metric_table_0_bit[l_ndx];
  *l_bit_1 = d_metric_table_1_bit[l_ndx];
}

template<typename pdf_fcn_io_t, typename metric_t>
void
code_metrics_table_work<pdf_fcn_io_t,metric_t>::convert
(size_t n_syms,
 pdf_fcn_io_t* sym,
 void* bit_0,
 void* bit_1)
{
  metric_ptr_t l_bit_0 = (metric_ptr_t) bit_0;
  metric_ptr_t l_bit_1 = (metric_ptr_t) bit_1;

  for (size_t m = n_syms; m > 0; m--)
    lookup (*sym++, (void*) l_bit_0++, (void*) l_bit_1++);
}

// force the compiler to instantiate a particular version of the
// templated super-class, for <float> PDF function precision because
// all code_metrics classes are created by this function, this is the
// only one which is required to instantaite.

template
code_metrics_table<float>*
libecc_code_metrics_create_table<float>
(float (*pdf_fcn_0_bit) (float),
 float (*pdf_fcn_1_bit) (float),
 size_t n_samples,
 float min_sample,
 float max_sample,
 int sample_precision);

#if 0
  // for compute_all_outputs

  d_n_code_outputs = n_code_outputs;

  in_l[0].resize (d_n_code_outputs);
  in_l[1].resize (d_n_code_outputs);
  in_f[0].resize (d_n_code_outputs);
  in_f[1].resize (d_n_code_outputs);



  if (n_code_outputs == 0) {
    std::cerr << "code_metrics::create: Error: # of code outputs "
      "must be positive.\n";
    assert (0);
  }


template<typename pdf_fcn_io_t>
void
code_metrics<pdf_fcn_io_t>::compute_all_outputs
(pdf_fcn_io_t* syms,
 std::vector<unsigned long>& out)
{
  // use the first 'n_code_output' symbols, convert them into metrics,
  // then compute all possible (summation) combinations of them and
  // return those in the provided vector.

  convert (d_n_code_outputs, syms,
	   (void*)(&(in_l[0][0])), (void*)(&in_l[1][0]));

  // assign the starting minimum metric to 0.  This is safe because
  // metrics are always non-negative.

  unsigned long min_metric = 0;
  for (size_t m = 0; m < (((size_t)2) << d_n_code_outputs); m++) {
    size_t t_out_ndx = m;
    unsigned long t_metric = 0;
    for (size_t n = 0; n < d_n_code_outputs; n++, t_out_ndx >>= 1)
      t_metric += in_l[t_out_ndx&1][n];
    if (t_metric < min_metric)
      min_metric = t_metric;
    out[m] = t_metric;
  }

  // normalize so that the minimum metric equals 0

  for (size_t m = 0; m < d_n_code_outputs; m++)
    out[m] -= min_metric;
}

template<typename pdf_fcn_io_t>
void
code_metrics<pdf_fcn_io_t>::compute_all_outputs
(pdf_fcn_io_t* syms,
 std::vector<float>& out)
{
  // use the first 'n_code_output' symbols, convert them into metrics,
  // then compute all possible (summation) combinations of them and
  // return those in the provided vector.

  convert (d_n_code_outputs, syms,
	   (void*)(&(in_f[0][0])), (void*)(&in_f[1][0]));

  // assign the starting minimum metric to 0.  This is safe because
  // metrics are always non-negative.

  float min_metric = 0;
  for (size_t m = 0; m < (((size_t)2) << d_n_code_outputs); m++) {
    size_t t_out_ndx = m;
    float t_metric = 0;
    for (size_t n = 0; n < d_n_code_outputs; n++, t_out_ndx >>= 1)
      t_metric += in_f[t_out_ndx&1][n];
    if (t_metric < min_metric)
      min_metric = t_metric;
    out[m] = t_metric;
  }

  // normalize so that the minimum metric equals 0

  for (size_t m = 0; m < d_n_code_outputs; m++)
    out[m] -= min_metric;
}
#endif
