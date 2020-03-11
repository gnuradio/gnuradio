/* -*- c++ -*- */
/*
 * Copyright 2004,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef ENCODER_IMPL_H
#define ENCODER_IMPL_H

#include <gnuradio/trellis/encoder.h>

namespace gr {
namespace trellis {

template <class IN_T, class OUT_T>
class encoder_impl : public encoder<IN_T, OUT_T>
{
private:
    fsm d_FSM;
    int d_ST;
    int d_K;
    bool d_B;


public:
    encoder_impl(const fsm& FSM, int ST, int K, bool B);
    ~encoder_impl();

    fsm FSM() const
    {
        return d_FSM;
        ;
    }
    int ST() const { return d_ST; }
    int K() const { return d_K; }
    void set_FSM(const fsm& FSM);
    void set_ST(int ST);
    void set_K(int K);
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace trellis */
} /* namespace gr */

#endif /* ENCODER_IMPL_H */
