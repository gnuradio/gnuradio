/* -*- c++ -*- */
/*
 * Copyright 2004,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef PCCC_DECODER_BLK_IMPL_H
#define PCCC_DECODER_BLK_IMPL_H

#include <gnuradio/trellis/pccc_decoder_blk.h>

namespace gr {
namespace trellis {

template <class T>
class pccc_decoder_blk_impl : public pccc_decoder_blk<T>
{
private:
    fsm d_FSM1;
    int d_ST10;
    int d_ST1K;
    fsm d_FSM2;
    int d_ST20;
    int d_ST2K;
    interleaver d_INTERLEAVER;
    int d_blocklength;
    int d_repetitions;
    siso_type_t d_SISO_TYPE;
    std::vector<float> d_buffer;

public:
    pccc_decoder_blk_impl(const fsm& FSM1,
                          int ST10,
                          int ST1K,
                          const fsm& FSM2,
                          int ST20,
                          int ST2K,
                          const interleaver& INTERLEAVER,
                          int blocklength,
                          int repetitions,
                          siso_type_t SISO_TYPE);
    ~pccc_decoder_blk_impl() override;

    fsm FSM1() const override { return d_FSM1; }
    fsm FSM2() const override { return d_FSM2; }
    int ST10() const override { return d_ST10; }
    int ST1K() const override { return d_ST1K; }
    int ST20() const override { return d_ST20; }
    int ST2K() const override { return d_ST2K; }
    interleaver INTERLEAVER() const override { return d_INTERLEAVER; }
    int blocklength() const override { return d_blocklength; }
    int repetitions() const override { return d_repetitions; }
    siso_type_t SISO_TYPE() const override { return d_SISO_TYPE; }

    void forecast(int noutput_items, gr_vector_int& ninput_items_required) override;

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;
};

} /* namespace trellis */
} /* namespace gr */

#endif /* PCCC_DECODER_BLK_IMPL_H */
