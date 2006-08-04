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

#ifndef INCLUDED_CODE_METRICS_H
#define INCLUDED_CODE_METRICS_H

#include "code_types.h"
#include <vector>

template<typename pdf_fcn_io_t>
class code_metrics_table;

template<typename pdf_fcn_io_t>
code_metrics_table<pdf_fcn_io_t>*
libecc_code_metrics_create_table
(pdf_fcn_io_t (*pdf_fcn_0_bit) (pdf_fcn_io_t),
 pdf_fcn_io_t (*pdf_fcn_1_bit) (pdf_fcn_io_t),
 size_t n_samples,
 pdf_fcn_io_t min_sample,
 pdf_fcn_io_t max_sample,
 int sample_precision = 0);

#if 0
template<typename pdf_fcn_io_t>
class code_metrics_decoder;

template<typename pdf_fcn_io_t>
libecc_code_metrics_create_decoder
(code_convolutional_trellis* trellis,
 code_metrics_table<pdf_fcn_io_t>* table = 0,
 int sample_precision = 0);
#endif

template<typename pdf_fcn_io_t>
class code_metrics_table
{
/*
 * class code_metrics_table: metrics table for convolutional codes.
 * Pre-compute a lookup table upon instantiation, which makes for
 * quick conversion from soft-float symbol value ('sym') to metric
 * value.  The symbol value probability is defined by the arguments
 * 'pdf_fcn_0_bit' and 'pdf_fcn_1_bit' for p(0|sym) and p(1|sym)
 * respectively.  Internally, the PDF's are sampled and normalized to
 * always have a minimum value of 0.  For float-precision, the maximum
 * value is 1.0, while for integer M-bits the maximum value is
 * (2^M)-1.  Smaller metric values indicate that the received symbols
 * are closer to the given output bits; larger values indicate greater
 * differences.  The only constraint on the PDF functions are that
 * they are piecewise continuous; otherwise, they don't even have to
 * be a true PDF in terms of the integral from -inf to +inf being
 * equal to 1 due to the normalization.
 *
 * Storage type for the tables is determined by the "sample_precision"
 * argument.  When the precision equals 0, 32-bit float storage is
 * used; otherwise, the next largest standard integer type is used
 * unsigned (char for 1 to 8 bits, short for 9 to 16 bits, and long
 * for 17 to 32 bits).  For the purposes of coding gain, any
 * sample_precision larger than about 9 will have minimal added
 * benefit under most conditions where communications are possible.
 * Trellis computations are performed in either 32-bit float (for
 * float storage) or 32-bit unsigned long (for all integer storage).
 *
 * The number of samples to store is determined by the "n_samples"
 * argument, which must be at least 2 but is otherwise not limited
 * except by the memory of the host computer.
 *
 * Samples of the PDF functions are taken from "min_sample" to
 * "max_sample", which represent the floor and ceiling on input symbol
 * values below and above which symbol values are truncated.
 * Internally, a sub-n_samples value is determined and used to "sum"
 * the PDF functions to divide the probabilities into "bins".
 */

public:
  typedef pdf_fcn_io_t (*pdf_fcn_t) (pdf_fcn_io_t);

  virtual ~code_metrics_table () {};

  // lookup() returns either a float, or a sign-extended
  // 'sample_precision'-bit integer value.

  virtual void lookup (pdf_fcn_io_t sym,
		       void* bit_0,
		       void* bit_1) = 0;

  // convert does a lookup on 'n_syms' input symbols

  virtual void convert (size_t n_syms,
			pdf_fcn_io_t* syms,
			void* bit_0,
			void* bit_1) = 0;

  inline const unsigned char out_item_size_bytes ()
  {return(d_out_item_size_bytes);};

protected:
  code_metrics_table (pdf_fcn_t pdf_fcn_0_bit,
		      pdf_fcn_t pdf_fcn_1_bit,
		      size_t n_samples,
		      pdf_fcn_io_t min_sample,
		      pdf_fcn_io_t max_sample);

  unsigned char d_out_item_size_bytes, d_sample_precision;
  size_t d_n_samples;
  pdf_fcn_io_t d_max_sample, d_min_sample, d_delta;
  pdf_fcn_t d_pdf_fcn_0_bit, d_pdf_fcn_1_bit;
  std::vector<pdf_fcn_io_t> d_pdf_fcn_0_samples, d_pdf_fcn_1_samples;
};

template<typename pdf_fcn_io_t, typename metric_t>
class code_metrics_table_work : public code_metrics_table<pdf_fcn_io_t>
{
public:
  typedef metric_t *metric_ptr_t;
  typedef pdf_fcn_io_t (*pdf_fcn_t) (pdf_fcn_io_t);

  ~code_metrics_table_work () {};

  void lookup (pdf_fcn_io_t sym, void* bit_0, void* bit_1);
  void convert (size_t n_syms, pdf_fcn_io_t* sym, void* bit_0, void* bit_1);

protected:
  code_metrics_table_work (pdf_fcn_t pdf_fcn_0_bit,
			   pdf_fcn_t pdf_fcn_1_bit,
			   size_t n_samples,
			   pdf_fcn_io_t min_sample,
			   pdf_fcn_io_t max_sample,
			   int sample_precision = 0);

  friend code_metrics_table<pdf_fcn_io_t>*
  libecc_code_metrics_create_table<pdf_fcn_io_t>
  (pdf_fcn_io_t (*pdf_fcn_0_bit) (pdf_fcn_io_t),
   pdf_fcn_io_t (*pdf_fcn_1_bit) (pdf_fcn_io_t),
   size_t n_samples,
   pdf_fcn_io_t min_sample,
   pdf_fcn_io_t max_sample,
   int sample_precision);

  std::vector<metric_t> d_metric_table_0_bit, d_metric_table_1_bit;
};

#if 0
  // compute all output-bit combinations of the incoming symbols' metrics

  void compute_all_outputs (pdf_fcn_io_t* syms, std::vector<unsigned long>& out);
  void compute_all_outputs (pdf_fcn_io_t* syms, std::vector<float>& out);

  size_t d_n_code_outputs;
  std::vector<unsigned long> in_l[2];
  std::vector<float> in_f[2];
#endif

#endif /* INCLUDED_CODE_METRICS_H */
