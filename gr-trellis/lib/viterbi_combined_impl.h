/* -*- c++ -*- */
/*
 * Copyright 2004,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef VITERBI_COMBINED_IMPL_H
#define VITERBI_COMBINED_IMPL_H

#include <gnuradio/trellis/viterbi_combined.h>

namespace gr {
namespace trellis {

template <class IN_T, class OUT_T>
class viterbi_combined_impl : public viterbi_combined<IN_T, OUT_T>
{
private:
    fsm d_FSM;
    int d_K;
    int d_S0;
    int d_SK;
    int d_D;
    std::vector<IN_T> d_TABLE;
    digital::trellis_metric_type_t d_TYPE;
    // std::vector<int> d_trace;

public:
    viterbi_combined_impl(const fsm& FSM,
                          int K,
                          int S0,
                          int SK,
                          int D,
                          const std::vector<IN_T>& TABLE,
                          digital::trellis_metric_type_t TYPE);
    ~viterbi_combined_impl();

    fsm FSM() const { return d_FSM; }
    int K() const { return d_K; }
    int S0() const { return d_S0; }
    int SK() const { return d_SK; }
    int D() const { return d_D; }
    std::vector<IN_T> TABLE() const { return d_TABLE; }
    digital::trellis_metric_type_t TYPE() const { return d_TYPE; }
    // std::vector<int> trace() const { return d_trace; }

    void set_FSM(const fsm& FSM);
    void set_K(int K);
    void set_S0(int S0);
    void set_SK(int SK);
    void set_D(int D);
    void set_TABLE(const std::vector<IN_T>& table);
    void set_TYPE(digital::trellis_metric_type_t type);

    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
};

} /* namespace trellis */
} /* namespace gr */

#endif /* VITERBI_COMBINED_IMPL_H */
