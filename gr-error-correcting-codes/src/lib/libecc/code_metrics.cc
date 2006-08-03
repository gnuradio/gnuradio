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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <code_metrics.h>
#include <iostream>
#include <math.h>
#include <assert.h>

code_metric_ff::code_metric_ff
(pdf_fcn_t pdf_fcn_0_bit,
 pdf_fcn_t pdf_fcn_1_bit,
 size_t n_samples,
 pdf_fcn_io_t min_sample,
 pdf_fcn_io_t max_sample)
{
  if (n_samples < 2) {
    fprintf (stderr, "code_metric_f32:: n_samples "
	     "must be at least 2.\n");
    assert (0);
  }
  if (min_sample >= max_sample) {
    fprintf (stderr, "code_metric_f32:: min_sample must be "
	     "less than max_sample.\n");
    assert (0);
  }
  if (! pdf_fcn_0_bit) {
    fprintf (stderr, "code_metric_f32:: pdf_fcn_0_bit must be "
	     "a non-null pointer to function.\n");
    assert (0);
  }
  if (! pdf_fcn_1_bit) {
    fprintf (stderr, "code_metric_f32:: pdf_fcn_0_bit must be "
	     "a non-null pointer to function.\n");
    assert (0);
  }

  d_n_samples = n_samples;
  d_max_sample = max_sample;
  d_min_sample = min_sample;
  d_delta = (max_sample - min_sample) / (n_samples - 1);
  d_pdf_fcn_0_bit = pdf_fcn_0_bit;
  d_pdf_fcn_1_bit = pdf_fcn_1_bit;
  d_metric_table_0_bit.assign (n_samples, 0);
  d_metric_table_1_bit.assign (n_samples, 0);

  pdf_fcn_io_t l_val = min_sample;
  for (size_t m = 0; m < n_samples; m++) {
    d_metric_table_0_bit[m] = logf ((*pdf_fcn_0_bit)(l_val));
    d_metric_table_1_bit[m] = logf ((*pdf_fcn_1_bit)(l_val));
    l_val += d_delta;
  }
}

void code_metric_ff::lookup
(pdf_fcn_io_t sym,
 void* bit_0,
 void* bit_1)
{
  metric_ptr_t l_bit_0 = (metric_ptr_t) bit_0;
  metric_ptr_t l_bit_1 = (metric_ptr_t) bit_1;

  if (sym <= d_min_sample) {
    *l_bit_0 = d_metric_table_0_bit[0];
    *l_bit_1 = d_metric_table_1_bit[0];
    return;
  }
  if (sym >= d_max_sample) {
    *l_bit_0 = d_metric_table_0_bit.back ();
    *l_bit_1 = d_metric_table_1_bit.back ();
    return;
  }

  size_t l_ndx = (size_t) roundf ((sym - d_min_sample) / d_delta);
  *l_bit_0 = d_metric_table_0_bit[l_ndx];
  *l_bit_1 = d_metric_table_1_bit[l_ndx];
}

void code_metric_ff::convert
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

code_metric_fl::code_metric_fl
(pdf_fcn_t pdf_fcn_0_bit,
 pdf_fcn_t pdf_fcn_1_bit,
 size_t n_samples,
 pdf_fcn_io_t min_sample,
 pdf_fcn_io_t max_sample,
 int sample_precision)
{
  if (n_samples < 2) {
    fprintf (stderr, "code_metric_fl:: n_samples "
	     "must be at least 2.\n");
    assert (0);
  }
  if (min_sample >= max_sample) {
    fprintf (stderr, "code_metric_fl:: min_sample must be "
	     "less than max_sample.\n");
    assert (0);
  }
  if (! pdf_fcn_0_bit) {
    fprintf (stderr, "code_metric_fl:: pdf_fcn_0_bit must be "
	     "a non-null pointer to function.\n");
    assert (0);
  }
  if (! pdf_fcn_1_bit) {
    fprintf (stderr, "code_metric_fl:: pdf_fcn_0_bit must be "
	     "a non-null pointer to function.\n");
    assert (0);
  }
  if (sample_precision < 16 || sample_precision > 32) {
    fprintf (stderr, "code_metric_fl:: sample_precision must be "
	     "between 16 and 32 for this class.\n");
    assert (0);
  }

  d_sample_precision = sample_precision;
  d_n_samples = n_samples;
  d_max_sample = max_sample;
  d_min_sample = min_sample;
  d_delta = (max_sample - min_sample) / (n_samples - 1);
  d_pdf_fcn_0_bit = pdf_fcn_0_bit;
  d_pdf_fcn_1_bit = pdf_fcn_1_bit;
  d_metric_table_0_bit.assign (n_samples, 0);
  d_metric_table_1_bit.assign (n_samples, 0);

  // get the scale factor for converting from float to sample_precision
  // maps:
  //    logf (pdf_fcn_0_bit->eval (d_min_sample))  ->  l_min_map
  //    logf (pdf_fcn_0_bit->eval (d_max_sample))  ->  l_max_map

  metric_t l_min_map = - (1 << (sample_precision - 1));

  pdf_fcn_io_t l_min_log_val_0 = logf ((*pdf_fcn_0_bit)(d_min_sample));
  pdf_fcn_io_t l_max_log_val_0 = logf ((*pdf_fcn_0_bit)(d_max_sample));
  pdf_fcn_io_t l_slope_0 = (powf (2.0, (pdf_fcn_io_t)(d_sample_precision)) /
			    (l_max_log_val_0 - l_min_log_val_0));
  pdf_fcn_io_t l_min_log_val_1 = logf ((*pdf_fcn_1_bit)(d_min_sample));
  pdf_fcn_io_t l_max_log_val_1 = logf ((*pdf_fcn_1_bit)(d_max_sample));
  pdf_fcn_io_t l_slope_1 = (powf (2.0, (pdf_fcn_io_t)(d_sample_precision)) /
			    (l_max_log_val_1 - l_min_log_val_1));

  pdf_fcn_io_t l_val = d_min_sample;
  for (size_t m = 0; m < d_n_samples; m++) {
    d_metric_table_0_bit[m] =
      (metric_t) roundf (((pdf_fcn_io_t) l_min_map) +
			 (l_slope_0 * (logf ((*pdf_fcn_0_bit)(l_val)) -
				       l_min_log_val_0)));
    d_metric_table_1_bit[m] =
      (metric_t) roundf (((pdf_fcn_io_t) l_min_map) +
			 (l_slope_1 * (logf ((*pdf_fcn_1_bit)(l_val)) -
				       l_min_log_val_1)));
    l_val += d_delta;
  }
}

void code_metric_fl::lookup
(pdf_fcn_io_t sym,
 void* bit_0,
 void* bit_1)
{
  metric_ptr_t l_bit_0 = (metric_ptr_t) bit_0;
  metric_ptr_t l_bit_1 = (metric_ptr_t) bit_1;

  if (sym <= d_min_sample) {
    *l_bit_0 = d_metric_table_0_bit[0];
    *l_bit_1 = d_metric_table_1_bit[0];
    return;
  }
  if (sym >= d_max_sample) {
    *l_bit_0 = d_metric_table_0_bit.back ();
    *l_bit_1 = d_metric_table_1_bit.back ();
    return;
  }

  size_t l_ndx = (size_t) roundf ((sym - d_min_sample) / d_delta);
  *l_bit_0 = d_metric_table_0_bit[l_ndx];
  *l_bit_1 = d_metric_table_1_bit[l_ndx];
}

void code_metric_fl::convert
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

code_metric_fs::code_metric_fs
(pdf_fcn_t pdf_fcn_0_bit,
 pdf_fcn_t pdf_fcn_1_bit,
 size_t n_samples,
 pdf_fcn_io_t min_sample,
 pdf_fcn_io_t max_sample,
 int sample_precision)
{
  if (n_samples < 2) {
    fprintf (stderr, "code_metric_fs:: n_samples "
	     "must be at least 2.\n");
    assert (0);
  }
  if (min_sample >= max_sample) {
    fprintf (stderr, "code_metric_fs:: min_sample must be "
	     "less than max_sample.\n");
    assert (0);
  }
  if (! pdf_fcn_0_bit) {
    fprintf (stderr, "code_metric_fs:: pdf_fcn_0_bit must be "
	     "a non-null pointer to function.\n");
    assert (0);
  }
  if (! pdf_fcn_1_bit) {
    fprintf (stderr, "code_metric_fs:: pdf_fcn_0_bit must be "
	     "a non-null pointer to function.\n");
    assert (0);
  }
  if (sample_precision < 9 || sample_precision > 16) {
    fprintf (stderr, "code_metric_fs:: sample_precision must be "
	     "between 9 and 16 for this class.\n");
    assert (0);
  }

  d_sample_precision = sample_precision;
  d_n_samples = n_samples;
  d_max_sample = max_sample;
  d_min_sample = min_sample;
  d_delta = (max_sample - min_sample) / (n_samples - 1);
  d_pdf_fcn_0_bit = pdf_fcn_0_bit;
  d_pdf_fcn_1_bit = pdf_fcn_1_bit;
  d_metric_table_0_bit.assign (n_samples, 0);
  d_metric_table_1_bit.assign (n_samples, 0);

  // get the scale factor for converting from float to sample_precision
  // maps:
  //    logf (pdf_fcn_0_bit->eval (d_min_sample))  ->  l_min_map
  //    logf (pdf_fcn_0_bit->eval (d_max_sample))  ->  l_max_map

  metric_t l_min_map = - (1 << (sample_precision - 1));

  pdf_fcn_io_t l_min_log_val_0 = logf ((*pdf_fcn_0_bit)(d_min_sample));
  pdf_fcn_io_t l_max_log_val_0 = logf ((*pdf_fcn_0_bit)(d_max_sample));
  pdf_fcn_io_t l_slope_0 = (powf (2.0, (pdf_fcn_io_t)(d_sample_precision)) /
			    (l_max_log_val_0 - l_min_log_val_0));
  pdf_fcn_io_t l_min_log_val_1 = logf ((*pdf_fcn_1_bit)(d_min_sample));
  pdf_fcn_io_t l_max_log_val_1 = logf ((*pdf_fcn_1_bit)(d_max_sample));
  pdf_fcn_io_t l_slope_1 = (powf (2.0, (pdf_fcn_io_t)(d_sample_precision)) /
			    (l_max_log_val_1 - l_min_log_val_1));

  pdf_fcn_io_t l_val = d_min_sample;
  for (size_t m = 0; m < d_n_samples; m++) {
    d_metric_table_0_bit[m] =
      (metric_t) roundf (((pdf_fcn_io_t) l_min_map) +
			 (l_slope_0 * (logf ((*pdf_fcn_0_bit)(l_val)) -
				       l_min_log_val_0)));
    d_metric_table_1_bit[m] =
      (metric_t) roundf (((pdf_fcn_io_t) l_min_map) +
			 (l_slope_1 * (logf ((*pdf_fcn_1_bit)(l_val)) -
				       l_min_log_val_1)));
    l_val += d_delta;
  }
}

void code_metric_fs::lookup
(pdf_fcn_io_t sym,
 void* bit_0,
 void* bit_1)
{
  metric_ptr_t l_bit_0 = (metric_ptr_t) bit_0;
  metric_ptr_t l_bit_1 = (metric_ptr_t) bit_1;

  if (sym <= d_min_sample) {
    *l_bit_0 = d_metric_table_0_bit[0];
    *l_bit_1 = d_metric_table_1_bit[0];
    return;
  }
  if (sym >= d_max_sample) {
    *l_bit_0 = d_metric_table_0_bit.back ();
    *l_bit_1 = d_metric_table_1_bit.back ();
    return;
  }

  size_t l_ndx = (size_t) roundf ((sym - d_min_sample) / d_delta);
  *l_bit_0 = d_metric_table_0_bit[l_ndx];
  *l_bit_1 = d_metric_table_1_bit[l_ndx];
}

void code_metric_fs::convert
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

code_metric_fb::code_metric_fb
(pdf_fcn_t pdf_fcn_0_bit,
 pdf_fcn_t pdf_fcn_1_bit,
 size_t n_samples,
 pdf_fcn_io_t min_sample,
 pdf_fcn_io_t max_sample,
 int sample_precision)
{
  if (n_samples < 2) {
    fprintf (stderr, "code_metric_fb:: n_samples "
	     "must be at least 2.\n");
    assert (0);
  }
  if (min_sample >= max_sample) {
    fprintf (stderr, "code_metric_fb:: min_sample must be "
	     "less than max_sample.\n");
    assert (0);
  }
  if (! pdf_fcn_0_bit) {
    fprintf (stderr, "code_metric_fb:: pdf_fcn_0_bit must be "
	     "a non-null pointer to function.\n");
    assert (0);
  }
  if (! pdf_fcn_1_bit) {
    fprintf (stderr, "code_metric_fb:: pdf_fcn_0_bit must be "
	     "a non-null pointer to function.\n");
    assert (0);
  }
  if (sample_precision < 1 || sample_precision > 8) {
    fprintf (stderr, "code_metric_fb:: sample_precision must be "
	     "between 1 and 8 for this class.\n");
    assert (0);
  }

  d_sample_precision = sample_precision;
  d_n_samples = n_samples;
  d_max_sample = max_sample;
  d_min_sample = min_sample;
  d_delta = (max_sample - min_sample) / (n_samples - 1);
  d_pdf_fcn_0_bit = pdf_fcn_0_bit;
  d_pdf_fcn_1_bit = pdf_fcn_1_bit;
  d_metric_table_0_bit.assign (n_samples, 0);
  d_metric_table_1_bit.assign (n_samples, 0);

  // get the scale factor for converting from float to sample_precision
  // maps:
  //    logf (pdf_fcn_0_bit->eval (d_min_sample))  ->  l_min_map
  //    logf (pdf_fcn_0_bit->eval (d_max_sample))  ->  l_max_map

  metric_t l_min_map = - (1 << (sample_precision - 1));

  pdf_fcn_io_t l_min_log_val_0 = logf ((*pdf_fcn_0_bit)(d_min_sample));
  pdf_fcn_io_t l_max_log_val_0 = logf ((*pdf_fcn_0_bit)(d_max_sample));
  pdf_fcn_io_t l_slope_0 = (powf (2.0, (pdf_fcn_io_t)(d_sample_precision)) /
			    (l_max_log_val_0 - l_min_log_val_0));
  pdf_fcn_io_t l_min_log_val_1 = logf ((*pdf_fcn_1_bit)(d_min_sample));
  pdf_fcn_io_t l_max_log_val_1 = logf ((*pdf_fcn_1_bit)(d_max_sample));
  pdf_fcn_io_t l_slope_1 = (powf (2.0, (pdf_fcn_io_t)(d_sample_precision)) /
			    (l_max_log_val_1 - l_min_log_val_1));

  pdf_fcn_io_t l_val = d_min_sample;
  for (size_t m = 0; m < d_n_samples; m++) {
    d_metric_table_0_bit[m] =
      (metric_t) roundf (((pdf_fcn_io_t) l_min_map) +
			 (l_slope_0 * (logf ((*pdf_fcn_0_bit)(l_val)) -
				       l_min_log_val_0)));
    d_metric_table_1_bit[m] =
      (metric_t) roundf (((pdf_fcn_io_t) l_min_map) +
			 (l_slope_1 * (logf ((*pdf_fcn_1_bit)(l_val)) -
				       l_min_log_val_1)));
    l_val += d_delta;
  }
}

void code_metric_fb::lookup
(pdf_fcn_io_t sym,
 void* bit_0,
 void* bit_1)
{
  metric_ptr_t l_bit_0 = (metric_ptr_t) bit_0;
  metric_ptr_t l_bit_1 = (metric_ptr_t) bit_1;

  if (sym <= d_min_sample) {
    *l_bit_0 = d_metric_table_0_bit[0];
    *l_bit_1 = d_metric_table_1_bit[0];
    return;
  }
  if (sym >= d_max_sample) {
    *l_bit_0 = d_metric_table_0_bit.back ();
    *l_bit_1 = d_metric_table_1_bit.back ();
    return;
  }

  size_t l_ndx = (size_t) roundf ((sym - d_min_sample) / d_delta);
  *l_bit_0 = d_metric_table_0_bit[l_ndx];
  *l_bit_1 = d_metric_table_1_bit[l_ndx];
}

void code_metric_fb::convert
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
