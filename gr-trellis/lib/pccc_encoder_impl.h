/* -*- c++ -*- */
/*
 * Copyright 2004,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef PCCC_ENCODER_IMPL_H
#define PCCC_ENCODER_IMPL_H

#include <gnuradio/trellis/pccc_encoder.h>

namespace gr {
namespace trellis {

template <class IN_T, class OUT_T>
class pccc_encoder_impl : public pccc_encoder<IN_T, OUT_T>
{
private:
    fsm d_FSM1;
    int d_ST1;
    fsm d_FSM2;
    int d_ST2;
    interleaver d_INTERLEAVER;
    int d_blocklength;
    std::vector<int> d_buffer;

public:
    pccc_encoder_impl(const fsm& FSM1,
                      int ST1,
                      const fsm& FSM2,
                      int ST2,
                      const interleaver& INTERLEAVER,
                      int blocklength);
    ~pccc_encoder_impl();

    fsm FSM1() const { return d_FSM1; }
    int ST1() const { return d_ST1; }
    fsm FSM2() const { return d_FSM2; }
    int ST2() const { return d_ST2; }
    interleaver INTERLEAVER() const { return d_INTERLEAVER; }
    int blocklength() const { return d_blocklength; }

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace trellis */
} /* namespace gr */

#endif /* PCCC_ENCODER_IMPL_H */
