/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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

// WARNING: this file is machine generated.  Edits will be over written

#ifndef INCLUDED_TRELLIS_METRICS_I_H
#define INCLUDED_TRELLIS_METRICS_I_H

#include <gr_block.h>
#include "trellis_calc_metric.h"

class trellis_metrics_i;
typedef boost::shared_ptr<trellis_metrics_i> trellis_metrics_i_sptr;

trellis_metrics_i_sptr trellis_make_metrics_i (int O, int D,  const std::vector<int> &TABLE, trellis_metric_type_t TYPE);

/*!
 * \brief Evaluate metrics for use by the Viterbi algorithm.
 * \ingroup block
 *
 */
class trellis_metrics_i : public gr_block
{
  int d_O;
  int d_D;
  trellis_metric_type_t d_TYPE;
  std::vector<int> d_TABLE;

  friend trellis_metrics_i_sptr trellis_make_metrics_i (int O, int D,  const std::vector<int> &TABLE, trellis_metric_type_t TYPE);
  trellis_metrics_i (int O, int D,  const std::vector<int> &TABLE, trellis_metric_type_t TYPE);

public:
  int O () const { return d_O; }
  int D () const { return d_D; }
  trellis_metric_type_t TYPE () const { return d_TYPE; }
  std::vector<int> TABLE () const { return d_TABLE; }
  void forecast (int noutput_items,
		 gr_vector_int &ninput_items_required);
  int general_work (int noutput_items,
		    gr_vector_int &ninput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items);
};


#endif
