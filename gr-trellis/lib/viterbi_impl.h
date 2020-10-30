/* -*- c++ -*- */
/*
 * Copyright 2004,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef VITERBI_IMPL_H
#define VITERBI_IMPL_H

#include <gnuradio/trellis/viterbi.h>

namespace gr {
namespace trellis {

template <class T>
class viterbi_impl : public viterbi<T>
{
private:
    fsm d_FSM;
    int d_K;
    int d_S0;
    int d_SK;
    // std::vector<int> d_trace;

public:
    viterbi_impl(const fsm& FSM, int K, int S0, int SK);
    ~viterbi_impl() override;

    fsm FSM() const override { return d_FSM; }
    int K() const override { return d_K; }
    int S0() const override { return d_S0; }
    int SK() const override { return d_SK; }

    void set_FSM(const fsm& FSM) override;
    void set_K(int K) override;
    void set_S0(int S0) override;
    void set_SK(int SK) override;
    // std::vector<int> trace () const { return d_trace; }

    void forecast(int noutput_items, gr_vector_int& ninput_items_required) override;
    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;
};

} /* namespace trellis */
} /* namespace gr */

#endif /* VITERBI_IMPL_H */
