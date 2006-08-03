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

#include <gr_syms_to_metrics.h>
#include <gr_io_signature.h>
#include <assert.h>

gr_syms_to_metrics_sptr 
gr_make_syms_to_metrics (gr_feval_ff* pdf_fcn_0_bit,
			 gr_feval_ff* pdf_fcn_1_bit,
			 int n_samples,
			 float min_sample,
			 float max_sample,
			 int sample_precision)
{
  return gr_syms_to_metrics_sptr
    (new gr_syms_to_metrics (pdf_fcn_0_bit,
			     pdf_fcn_1_bit,
			     n_samples,
			     min_sample,
			     max_sample,
			     sample_precision));
}

/*
 * dummy functions and variables to get the float(*)(float) function
 * to work properly with the gr_feval_XX stuff.
 */

static gr_feval_ff* l_pdf_fcn_0_bit;
static gr_feval_ff* l_pdf_fcn_1_bit;

static float pdf_fcn_0 (float x)
{
  return (l_pdf_fcn_0_bit->eval (x));
}

static float pdf_fcn_1 (float x)
{
  return (l_pdf_fcn_1_bit->eval (x));
}

gr_syms_to_metrics::gr_syms_to_metrics
(gr_feval_ff* pdf_fcn_0_bit,
 gr_feval_ff* pdf_fcn_1_bit,
 int n_samples,
 float min_sample,
 float max_sample,
 int sample_precision)
  : gr_block ("syms_to_metrics",
	      gr_make_io_signature (1, -1, sizeof (float)),
	      gr_make_io_signature (0, 0, 0))
{
  if ((sample_precision < 0) | (sample_precision > 32)) {
    fprintf (stderr, "gr_syms_to_metrics: sample_precision must be "
	     " between 0 and 32.\n");
    assert (0);
  }

  l_pdf_fcn_0_bit = pdf_fcn_0_bit;
  l_pdf_fcn_1_bit = pdf_fcn_1_bit;

  if (sample_precision == 0) {
    // float
    d_out_item_size_bytes = sizeof (float);
    d_code_metrics = new code_metric_ff (&pdf_fcn_0,
					 &pdf_fcn_1,
					 n_samples,
					 min_sample,
					 max_sample);
  } else if (sample_precision <= 8) {
    // use char
    d_out_item_size_bytes = sizeof (char);
    d_code_metrics = new code_metric_fb (&pdf_fcn_0,
					 &pdf_fcn_1,
					 n_samples,
					 min_sample,
					 max_sample,
					 sample_precision);
  } else if (sample_precision <= 16) {
    // use short
    d_out_item_size_bytes = sizeof (short);
    d_code_metrics = new code_metric_fs (&pdf_fcn_0,
					 &pdf_fcn_1,
					 n_samples,
					 min_sample,
					 max_sample,
					 sample_precision);
  } else {
    // use long
    d_out_item_size_bytes = sizeof (long);
    d_code_metrics = new code_metric_fl (&pdf_fcn_0,
					 &pdf_fcn_1,
					 n_samples,
					 min_sample,
					 max_sample,
					 sample_precision);
  }

  set_output_signature (gr_make_io_signature (1, -1, d_out_item_size_bytes));
}

bool gr_syms_to_metrics::check_topology (int ninputs, int noutputs)
{
  return ((noutputs == (2*ninputs)) ? true : false);
}

void
gr_syms_to_metrics::forecast
(int noutput_items,
 gr_vector_int &ninput_items_required)
{
  // always 1:1, for all streams
  for (size_t n = 0; n < ninput_items_required.size(); n++) {
    ninput_items_required[n] = noutput_items;
  }
}

int
gr_syms_to_metrics::general_work
(int noutput_items,
 gr_vector_int &ninput_items,
 gr_vector_const_void_star &input_items,
 gr_vector_void_star &output_items)
{
  size_t l_n_output_items = noutput_items;

  for (size_t n = 0; n < input_items.size(); n++) {
    float* t_in_buf = (float*)(&input_items[n]);
    void* t_out_buf_0_bit = (void*)(&(output_items[2*n]));
    void* t_out_buf_1_bit = (void*)(&(output_items[(2*n)+1]));

    d_code_metrics->convert (l_n_output_items, t_in_buf,
			     t_out_buf_0_bit, t_out_buf_1_bit);
  }

  // consume the number of used input items on all input streams

  consume_each (noutput_items);

  // returns number of items written to each output stream

  return (noutput_items);
}
