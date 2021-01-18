/* -*- c++ -*- */
/*
 * Copyright 2004,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <gnuradio/trellis/calc_metric.h>
#include <cfloat>
#include <stdexcept>

namespace gr {
namespace trellis {

template <class T>
void calc_metric(int O,
                 int D,
                 const std::vector<T>& TABLE,
                 const T* input,
                 float* metric,
                 digital::trellis_metric_type_t type)
{
    float minm = FLT_MAX;
    int minmi = 0;

    switch (type) {
    case digital::TRELLIS_EUCLIDEAN:
        for (int o = 0; o < O; o++) {
            metric[o] = 0.0;
            for (int m = 0; m < D; m++) {
                T s = input[m] - TABLE[o * D + m];
                // gr_complex sc(1.0*s,0);
                // metric[o] += (sc*conj(sc)).real();
                metric[o] += s * s;
            }
        }
        break;
    case digital::TRELLIS_HARD_SYMBOL:
        for (int o = 0; o < O; o++) {
            metric[o] = 0.0;
            for (int m = 0; m < D; m++) {
                T s = input[m] - TABLE[o * D + m];
                // gr_complex sc(1.0*s,0);
                // metric[o] + =(sc*conj(sc)).real();
                metric[o] += s * s;
            }
            if (metric[o] < minm) {
                minm = metric[o];
                minmi = o;
            }
        }
        for (int o = 0; o < O; o++) {
            metric[o] = (o == minmi ? 0.0 : 1.0);
        }
        break;
    case digital::TRELLIS_HARD_BIT:
        throw std::runtime_error(
            "calc_metric: Invalid metric type (not yet implemented).");
        break;
    default:
        throw std::runtime_error("calc_metric: Invalid metric type.");
    }
}

template void calc_metric<char>(int O,
                                int D,
                                const std::vector<char>& TABLE,
                                const char* input,
                                float* metric,
                                digital::trellis_metric_type_t type);

template void calc_metric<short>(int O,
                                 int D,
                                 const std::vector<short>& TABLE,
                                 const short* input,
                                 float* metric,
                                 digital::trellis_metric_type_t type);

template void calc_metric<int>(int O,
                               int D,
                               const std::vector<int>& TABLE,
                               const int* input,
                               float* metric,
                               digital::trellis_metric_type_t type);

template void calc_metric<float>(int O,
                                 int D,
                                 const std::vector<float>& TABLE,
                                 const float* input,
                                 float* metric,
                                 digital::trellis_metric_type_t type);

/*
void calc_metric(int O, int D, const std::vector<short> &TABLE, const short *input, float
*metric, digital::trellis_metric_type_t type)
{
  float minm = FLT_MAX;
  int minmi = 0;

  switch(type) {
  case digital::TRELLIS_EUCLIDEAN:
    for(int o = 0; o < O; o++) {
      metric[o] = 0.0;
      for(int m = 0; m < D; m++) {
        float s = input[m]-TABLE[o*D+m];
        metric[o] += s*s;
      }
    }
    break;
  case digital::TRELLIS_HARD_SYMBOL:
    for(int o = 0; o < O; o++) {
      metric[o] = 0.0;
      for(int m = 0; m < D; m++) {
        float s = input[m]-TABLE[o*D+m];
        metric[o] += s*s;
      }
      if(metric[o] < minm) {
        minm = metric[o];
        minmi = o;
      }
    }
    for(int o = 0; o < O; o++) {
      metric[o] = (o == minmi ? 0.0 : 1.0);
    }
    break;
  case digital::TRELLIS_HARD_BIT:
    throw std::runtime_error("calc_metric: Invalid metric type (not yet implemented).");
    break;
  default:
    throw std::runtime_error("calc_metric: Invalid metric type.");
  }
}

void calc_metric(int O, int D, const std::vector<int> &TABLE, const int *input,
                 float *metric, digital::trellis_metric_type_t type)
{
  float minm = FLT_MAX;
  int minmi = 0;

  switch(type){
  case digital::TRELLIS_EUCLIDEAN:
    for(int o=0;o<O;o++) {
      metric[o]=0.0;
      for(int m=0;m<D;m++) {
        float s=input[m]-TABLE[o*D+m];
        metric[o]+=s*s;
      }
    }
    break;
  case digital::TRELLIS_HARD_SYMBOL:
    for(int o=0;o<O;o++) {
      metric[o]=0.0;
      for(int m=0;m<D;m++) {
        float s=input[m]-TABLE[o*D+m];
        metric[o]+=s*s;
      }
      if(metric[o]<minm) {
        minm=metric[o];
        minmi=o;
      }
    }
    for(int o=0;o<O;o++) {
      metric[o] = (o==minmi?0.0:1.0);
    }
    break;
  case digital::TRELLIS_HARD_BIT:
    throw std::runtime_error("calc_metric: Invalid metric type (not yet implemented).");
    break;
  default:
    throw std::runtime_error("calc_metric: Invalid metric type.");
  }
}

void calc_metric(int O, int D, const std::vector<float> &TABLE, const float *input,
                 float *metric, digital::trellis_metric_type_t type)
{
  float minm = FLT_MAX;
  int minmi = 0;

  switch(type) {
  case digital::TRELLIS_EUCLIDEAN:
    for(int o=0;o<O;o++) {
      metric[o]=0.0;
      for(int m=0;m<D;m++) {
        float s=input[m]-TABLE[o*D+m];
        metric[o]+=s*s;
      }
    }
    break;
  case digital::TRELLIS_HARD_SYMBOL:
    for(int o=0;o<O;o++) {
      metric[o]=0.0;
      for(int m=0;m<D;m++) {
        float s=input[m]-TABLE[o*D+m];
        metric[o]+=s*s;
      }
      if(metric[o]<minm) {
        minm=metric[o];
        minmi=o;
      }
    }
    for(int o=0;o<O;o++) {
      metric[o] = (o==minmi?0.0:1.0);
    }
    break;
  case digital::TRELLIS_HARD_BIT:
    throw std::runtime_error("calc_metric: Invalid metric type (not yet implemented).");
    break;
  default:
    throw std::runtime_error("calc_metric: Invalid metric type.");
  }
}
*/

void calc_metric(int O,
                 int D,
                 const std::vector<gr_complex>& TABLE,
                 const gr_complex* input,
                 float* metric,
                 digital::trellis_metric_type_t type)
{
    float minm = FLT_MAX;
    int minmi = 0;

    switch (type) {
    case digital::TRELLIS_EUCLIDEAN:
        for (int o = 0; o < O; o++) {
            metric[o] = 0.0;
            for (int m = 0; m < D; m++) {
                gr_complex s = input[m] - TABLE[o * D + m];
                metric[o] += s.real() * s.real() + s.imag() * s.imag();
            }
        }
        break;
    case digital::TRELLIS_HARD_SYMBOL:
        for (int o = 0; o < O; o++) {
            metric[o] = 0.0;
            for (int m = 0; m < D; m++) {
                gr_complex s = input[m] - TABLE[o * D + m];
                metric[o] += s.real() * s.real() + s.imag() * s.imag();
            }
            if (metric[o] < minm) {
                minm = metric[o];
                minmi = o;
            }
        }
        for (int o = 0; o < O; o++) {
            metric[o] = (o == minmi ? 0.0 : 1.0);
        }
        break;
    case digital::TRELLIS_HARD_BIT:
        throw std::runtime_error(
            "calc_metric: Invalid metric type (not yet implemented).");
        break;
    default:
        throw std::runtime_error("calc_metric: Invalid metric type.");
    }
}

} /* namespace trellis */
} /* namespace gr */
