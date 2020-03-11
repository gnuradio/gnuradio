/* -*- c++ -*- */
/*
 * Copyright 2004,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_CALC_METRIC_H
#define INCLUDED_CALC_METRIC_H

#include <gnuradio/digital/metric_type.h>
#include <gnuradio/gr_complex.h>
#include <vector>

namespace gr {
namespace trellis {

template <class T>
void calc_metric(int O,
                 int D,
                 const std::vector<T>& TABLE,
                 const T* input,
                 float* metric,
                 digital::trellis_metric_type_t type);

/*
  void calc_metric(int O, int D, const std::vector<short> &TABLE, const short *input,
  float *metric, digital::trellis_metric_type_t type);

  void calc_metric(int O, int D, const std::vector<int> &TABLE, const int *input,
  float *metric, digital::trellis_metric_type_t type);

  void calc_metric(int O, int D, const std::vector<float> &TABLE, const float *input,
  float *metric, digital::trellis_metric_type_t type);
*/

void calc_metric(int O,
                 int D,
                 const std::vector<gr_complex>& TABLE,
                 const gr_complex* input,
                 float* metric,
                 digital::trellis_metric_type_t type);

} /* namespace trellis */
} /* namespace gr */

#endif /* INCLUDED_CALC_METRIC_H */
