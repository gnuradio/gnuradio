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

#ifndef INCLUDED_CODE_METRIC_H
#define INCLUDED_CODE_METRIC_H

#include <sys/types.h>
#include <vector>

class code_metrics
{
public:
  typedef float pdf_fcn_io_t;

  code_metrics () {};
  virtual ~code_metrics () {};

// lookup() returns either a float, or a sign-extended
// 'sample_precision'-bit integer value.

  virtual void lookup (pdf_fcn_io_t sym,
		       void* bit_0,
		       void* bit_1) = 0;

  virtual void convert (size_t n_syms,
			pdf_fcn_io_t* syms,
			void* bit_0,
			void* bit_1) = 0;
};

class code_metric_ff : public code_metrics
{
  typedef pdf_fcn_io_t (*pdf_fcn_t) (pdf_fcn_io_t);
  typedef float metric_t, *metric_ptr_t;

private:
  size_t d_n_samples;
  pdf_fcn_io_t d_max_sample, d_min_sample, d_delta;
  pdf_fcn_t d_pdf_fcn_0_bit, d_pdf_fcn_1_bit;
  std::vector<metric_t> d_metric_table_0_bit;
  std::vector<metric_t> d_metric_table_1_bit;

public:
  code_metric_ff (pdf_fcn_t pdf_fcn_0_bit,
		  pdf_fcn_t pdf_fcn_1_bit,
		  size_t n_samples,
		  pdf_fcn_io_t min_sample,
		  pdf_fcn_io_t max_sample);
  ~code_metric_ff () {};

  void lookup (pdf_fcn_io_t sym, void* bit_0, void* bit_1);
  void convert (size_t n_syms, pdf_fcn_io_t* sym, void* bit_0, void* bit_1);
};

class code_metric_fl : public code_metrics
{
  typedef float pdf_fcn_io_t;
  typedef pdf_fcn_io_t (*pdf_fcn_t) (pdf_fcn_io_t);
  typedef long metric_t, *metric_ptr_t;

private:
  char d_sample_precision;
  size_t d_n_samples, d_sample_mask;
  pdf_fcn_io_t d_max_sample, d_min_sample, d_delta;
  pdf_fcn_t d_pdf_fcn_0_bit, d_pdf_fcn_1_bit;
  std::vector<metric_t> d_metric_table_0_bit;
  std::vector<metric_t> d_metric_table_1_bit;

public:
  code_metric_fl (pdf_fcn_t pdf_fcn_0_bit,
		  pdf_fcn_t pdf_fcn_1_bit,
		  size_t n_samples,
		  pdf_fcn_io_t min_sample,
		  pdf_fcn_io_t max_sample,
		  int sample_precision = 32);
  ~code_metric_fl () {};

  void lookup (pdf_fcn_io_t sym, void* bit_0, void* bit_1);
  void convert (size_t n_syms, pdf_fcn_io_t* sym, void* bit_0, void* bit_1);
};

class code_metric_fs : public code_metrics
{
  typedef float pdf_fcn_io_t;
  typedef pdf_fcn_io_t (*pdf_fcn_t) (pdf_fcn_io_t);
  typedef short metric_t, *metric_ptr_t;

private:
  char d_sample_precision;
  size_t d_n_samples, d_sample_mask;
  pdf_fcn_io_t d_max_sample, d_min_sample, d_delta;
  pdf_fcn_t d_pdf_fcn_0_bit, d_pdf_fcn_1_bit;
  std::vector<metric_t> d_metric_table_0_bit;
  std::vector<metric_t> d_metric_table_1_bit;

public:
  code_metric_fs (pdf_fcn_t pdf_fcn_0_bit,
		  pdf_fcn_t pdf_fcn_1_bit,
		  size_t n_samples,
		  pdf_fcn_io_t min_sample,
		  pdf_fcn_io_t max_sample,
		  int sample_precision = 16);
  ~code_metric_fs () {};

  void lookup (pdf_fcn_io_t sym, void* bit_0, void* bit_1);
  void convert (size_t n_syms, pdf_fcn_io_t* sym, void* bit_0, void* bit_1);
};

class code_metric_fb : public code_metrics
{
  typedef float pdf_fcn_io_t;
  typedef pdf_fcn_io_t (*pdf_fcn_t) (pdf_fcn_io_t);
  typedef char metric_t, *metric_ptr_t;

private:
  char d_sample_precision;
  size_t d_n_samples, d_sample_mask;
  pdf_fcn_io_t d_max_sample, d_min_sample, d_delta;
  pdf_fcn_t d_pdf_fcn_0_bit, d_pdf_fcn_1_bit;
  std::vector<metric_t> d_metric_table_0_bit;
  std::vector<metric_t> d_metric_table_1_bit;

public:
  code_metric_fb (pdf_fcn_t pdf_fcn_0_bit,
		  pdf_fcn_t pdf_fcn_1_bit,
		  size_t n_samples,
		  pdf_fcn_io_t min_sample,
		  pdf_fcn_io_t max_sample,
		  int sample_precision = 8);
  ~code_metric_fb () {};

  void lookup (pdf_fcn_io_t sym, void* bit_0, void* bit_1);
  void convert (size_t n_syms, pdf_fcn_io_t* sym, void* bit_0, void* bit_1);
};

#endif /* INCLUDED_CODE_METRIC_H */
