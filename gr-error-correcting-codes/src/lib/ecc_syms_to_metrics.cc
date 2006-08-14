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

#include <ecc_syms_to_metrics.h>
#include <gr_io_signature.h>
#include <assert.h>

ecc_syms_to_metrics_sptr 
ecc_make_syms_to_metrics (gr_feval_ff* pdf_fcn_0_bit,
			  gr_feval_ff* pdf_fcn_1_bit,
			  int n_samples,
			  float min_sample,
			  float max_sample,
			  int sample_precision)
{
  return ecc_syms_to_metrics_sptr
    (new ecc_syms_to_metrics (pdf_fcn_0_bit,
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

ecc_syms_to_metrics::ecc_syms_to_metrics
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
  // setup the dummy functions to do the conversion from the
  // python-provided feval classes to that which is needed by the
  // libecc's code_metrics classes.

  l_pdf_fcn_0_bit = pdf_fcn_0_bit;
  l_pdf_fcn_1_bit = pdf_fcn_1_bit;

  // use the static "create" member function to create the actual
  // code_metrics to use.

#if 0
  d_code_metrics_table = libecc_code_metrics_create_table<float>
    (&pdf_fcn_0,
     &pdf_fcn_1,
     n_samples,
     min_sample,
     max_sample,
     sample_precision);

  // get the output item size in bytes from the new code_metrics.

  d_out_item_size_bytes = d_code_metrics_table->out_item_size_bytes ();
#endif

  // set the output signature to match that which the code_metrics
  // will generate.

  set_output_signature (gr_make_io_signature (1, -1, d_out_item_size_bytes));
}

bool ecc_syms_to_metrics::check_topology (int ninputs, int noutputs)
{
  // there are 2 output streams per input stream; the first is ~to
  // log(p(0|x)) = log-probability of a 0-bit given the input symbol
  // 'x'; the second is ~to log(p(1|x)).

  return ((noutputs == (2*ninputs)) ? true : false);
}

void
ecc_syms_to_metrics::forecast
(int noutput_items,
 gr_vector_int &ninput_items_required)
{
  // always 1:1, for all streams
  for (size_t n = 0; n < ninput_items_required.size(); n++) {
    ninput_items_required[n] = noutput_items;
  }
}

int
ecc_syms_to_metrics::general_work
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

#if 0
    d_code_metrics_table->convert (l_n_output_items, t_in_buf,
				   t_out_buf_0_bit, t_out_buf_1_bit);
#endif
  }

  // consume the number of used input items on all input streams

  consume_each (noutput_items);

  // returns number of items written to each output stream

  return (noutput_items);
}
