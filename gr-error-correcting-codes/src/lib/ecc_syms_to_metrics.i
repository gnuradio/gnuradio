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

GR_SWIG_BLOCK_MAGIC(ecc,syms_to_metrics);

ecc_syms_to_metrics_sptr ecc_make_syms_to_metrics
(gr_feval_ff* pdf_fcn_0_bit,
 gr_feval_ff* pdf_fcn_1_bit,
 int n_samples,
 float min_sample, 
 float max_sample,
 int sample_precision);

class ecc_syms_to_metrics : public gr_block
{
  ecc_syms_to_metrics (gr_feval_ff* pdf_fcn_0_bit,
		       gr_feval_ff* pdf_fcn_1_bit,
		       int n_samples,
		       float min_sample, 
		       float max_sample,
		       int sample_precision);
};
