/* -*- c++ -*- */
/*
 * Copyright 2004,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_CORE_ALGORITHMS_H
#define INCLUDED_CORE_ALGORITHMS_H

#include <gnuradio/digital/metric_type.h>
#include <gnuradio/trellis/fsm.h>
#include <gnuradio/trellis/interleaver.h>
#include <cmath>
#include <vector>

namespace gr {
namespace trellis {

float min(float a, float b);
float min_star(float a, float b);

template <class T>
void viterbi_algorithm(int I,
                       int S,
                       int O,
                       const std::vector<int>& NS,
                       const std::vector<int>& OS,
                       const std::vector<std::vector<int>>& PS,
                       const std::vector<std::vector<int>>& PI,
                       int K,
                       int S0,
                       int SK,
                       const float* in,
                       T* out);

template <class Ti, class To>
void viterbi_algorithm_combined(int I,
                                int S,
                                int O,
                                const std::vector<int>& NS,
                                const std::vector<int>& OS,
                                const std::vector<std::vector<int>>& PS,
                                const std::vector<std::vector<int>>& PI,
                                int K,
                                int S0,
                                int SK,
                                int D,
                                const std::vector<Ti>& TABLE,
                                digital::trellis_metric_type_t TYPE,
                                const Ti* in,
                                To* out);

void siso_algorithm(int I,
                    int S,
                    int O,
                    const std::vector<int>& NS,
                    const std::vector<int>& OS,
                    const std::vector<std::vector<int>>& PS,
                    const std::vector<std::vector<int>>& PI,
                    int K,
                    int S0,
                    int SK,
                    bool POSTI,
                    bool POSTO,
                    float (*p2mymin)(float, float),
                    const float* priori,
                    const float* prioro,
                    float* post);

template <class T>
void siso_algorithm_combined(int I,
                             int S,
                             int O,
                             const std::vector<int>& NS,
                             const std::vector<int>& OS,
                             const std::vector<std::vector<int>>& PS,
                             const std::vector<std::vector<int>>& PI,
                             int K,
                             int S0,
                             int SK,
                             bool POSTI,
                             bool POSTO,
                             float (*p2mymin)(float, float),
                             int D,
                             const std::vector<T>& TABLE,
                             digital::trellis_metric_type_t TYPE,
                             const float* priori,
                             const T* observations,
                             float* post);

template <class T>
void sccc_decoder(const fsm& FSMo,
                  int STo0,
                  int SToK,
                  const fsm& FSMi,
                  int STi0,
                  int STiK,
                  const interleaver& INTERLEAVER,
                  int blocklength,
                  int iterations,
                  float (*p2mymin)(float, float),
                  const float* iprioro,
                  T* data);

template <class Ti, class To>
void sccc_decoder_combined(const fsm& FSMo,
                           int STo0,
                           int SToK,
                           const fsm& FSMi,
                           int STi0,
                           int STiK,
                           const interleaver& INTERLEAVER,
                           int blocklength,
                           int iterations,
                           float (*p2mymin)(float, float),
                           int D,
                           const std::vector<Ti>& TABLE,
                           digital::trellis_metric_type_t METRIC_TYPE,
                           float scaling,
                           const Ti* observations,
                           To* data);

template <class T>
void pccc_decoder(const fsm& FSM1,
                  int ST10,
                  int ST1K,
                  const fsm& FSM2,
                  int ST20,
                  int ST2K,
                  const interleaver& INTERLEAVER,
                  int blocklength,
                  int iterations,
                  float (*p2mymin)(float, float),
                  const float* cprioro,
                  T* data);

template <class Ti, class To>
void pccc_decoder_combined(const fsm& FSM1,
                           int ST10,
                           int ST1K,
                           const fsm& FSM2,
                           int ST20,
                           int ST2K,
                           const interleaver& INTERLEAVER,
                           int blocklength,
                           int iterations,
                           float (*p2mymin)(float, float),
                           int D,
                           const std::vector<Ti>& TABLE,
                           digital::trellis_metric_type_t METRIC_TYPE,
                           float scaling,
                           const Ti* observations,
                           To* data);

} /* namespace trellis */
} /* namespace gr */

#endif /* INCLUDED_CORE_ALGORITHMS_H */
