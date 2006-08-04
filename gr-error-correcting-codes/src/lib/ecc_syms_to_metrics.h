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

#ifndef INCLUDED_SYMS_TO_METRICS_H
#define INCLUDED_SYMS_TO_METRICS_H

#include <gr_block.h>
#include <gr_feval.h>
#include <vector>
#include <libecc/code_metrics.h>

class ecc_syms_to_metrics;
typedef boost::shared_ptr<ecc_syms_to_metrics> ecc_syms_to_metrics_sptr;

ecc_syms_to_metrics_sptr ecc_make_syms_to_metrics
(gr_feval_ff* pdf_fcn_0_bit,
 gr_feval_ff* pdf_fcn_1_bit,
 int n_samples,
 float min_sample,
 float max_sample,
 int sample_precision);

/*!
 * \brief Convert the input stream(s) of soft (float) symbols to
 * log-probability metrics of user-specified precision; output is 2
 * streams per input stream, each stream consisting of the metric for
 * receiving a 0-bit and 1-bit, respectively, with the lower-numbered
 * stream being the 0-bit.
 *
 * input: stream(s) of float; output: stream(s) of metrics
 */

class ecc_syms_to_metrics : public gr_block
{
protected:
  friend ecc_syms_to_metrics_sptr
  ecc_make_syms_to_metrics (gr_feval_ff* pdf_fcn_0_bit,
			    gr_feval_ff* pdf_fcn_1_bit,
			    int n_samples,
			    float min_sample,
			    float max_sample,
			    int sample_precision);

/*
 * ecc_syms_to_metrics: Convert the input soft (float) symbols into
 *     log-probabilities (metrics) for use in the convolutional
 *     decoder.  Samples the provided PDF function in 'n_samples'
 *     places from 'min_sample' to 'max_sample', takes the log of that
 *     value, then converts the result into a given precision and
 *     stores all results in a "handy dandy" lookup table for much
 *     faster processing.
 *
 * pdf_fcn_0_bit: point to a probability distribution function which
 *     takes a float and returns a float, for the 0-bit probabilities.
 *
 * pdf_fcn_1_bit: point to a probability distribution function which
 *     takes a float and returns a float, for the 1-bit probabilities.
 *
 * n_samples: the number of samples between min_sample and max_sample
 *     to store in the lookup table.  Must be at least 2, but
 *     otherwise is limited only by the amount of available memory;
 *     generally, 65536 (1 << 16) is plenty of samples.
 *
 * min_sample: the minimum value below which any incoming value is
 *     "rounded" up.
 *
 * max_sample: the maximum value above which any incoming value is
 *     "rounded" down.
 *
 * sample_precision: the precision with which to sample the returned
 *     value of the PDF function.
 *  +  Cannot be < 0 or > 32.
 *  +  "soft float" == 0
 *  + otherwise, convert to an integer of the given value
 *
 * in "work", finds the (linearly) closest sample value for the given
 *     input and outputs the metric for a 0-bit input on the first
 *     stream and a 1-bit input on the second stream.
 *
 */

  ecc_syms_to_metrics (gr_feval_ff* pdf_fcn_0_bit,
		       gr_feval_ff* pdf_fcn_1_bit,
		       int n_samples,
		       float min_sample,
		       float max_sample,
		       int sample_precision);

  size_t d_out_item_size_bytes;
  code_metrics_table<float>* d_code_metrics_table;

public:
  ~ecc_syms_to_metrics() {delete d_code_metrics_table;};

  bool check_topology (int ninputs, int noutputs);

  void forecast (int noutput_items, gr_vector_int &ninput_items_required);

  int general_work (int noutput_items,
		    gr_vector_int &ninput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items);
};

#endif /* INCLUDED_SYMS_TO_METRICS_H */
