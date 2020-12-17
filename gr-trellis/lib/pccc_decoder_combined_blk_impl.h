/* -*- c++ -*- */
/*
 * Copyright 2004,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef PCCC_DECODER_COMBINED_BLK_IMPL_H
#define PCCC_DECODER_COMBINED_BLK_IMPL_H

#include <gnuradio/trellis/pccc_decoder_combined_blk.h>

namespace gr {
namespace trellis {

template <class IN_T, class OUT_T>
class pccc_decoder_combined_blk_impl : public pccc_decoder_combined_blk<IN_T, OUT_T>
{
private:
    fsm d_FSMo;
    int d_STo0;
    int d_SToK;
    fsm d_FSMi;
    int d_STi0;
    int d_STiK;
    interleaver d_INTERLEAVER;
    int d_blocklength;
    int d_repetitions;
    siso_type_t d_SISO_TYPE;
    int d_D;
    std::vector<IN_T> d_TABLE;
    digital::trellis_metric_type_t d_METRIC_TYPE;
    float d_scaling;
    std::vector<float> d_buffer;

public:
    pccc_decoder_combined_blk_impl(const fsm& FSMo,
                                   int STo0,
                                   int SToK,
                                   const fsm& FSMi,
                                   int STi0,
                                   int STiK,
                                   const interleaver& INTERLEAVER,
                                   int blocklength,
                                   int repetitions,
                                   siso_type_t SISO_TYPE,
                                   int D,
                                   const std::vector<IN_T>& TABLE,
                                   digital::trellis_metric_type_t METRIC_TYPE,
                                   float scaling);
    ~pccc_decoder_combined_blk_impl() override;

    fsm FSM1() const override { return d_FSMo; }
    fsm FSM2() const override { return d_FSMi; }
    int ST10() const override { return d_STo0; }
    int ST1K() const override { return d_SToK; }
    int ST20() const override { return d_STi0; }
    int ST2K() const override { return d_STiK; }
    interleaver INTERLEAVER() const override { return d_INTERLEAVER; }
    int blocklength() const override { return d_blocklength; }
    int repetitions() const override { return d_repetitions; }
    int D() const override { return d_D; }
    std::vector<IN_T> TABLE() const override { return d_TABLE; }
    digital::trellis_metric_type_t METRIC_TYPE() const override { return d_METRIC_TYPE; }
    siso_type_t SISO_TYPE() const override { return d_SISO_TYPE; }
    float scaling() const override { return d_scaling; }
    void set_scaling(float scaling) override;

    void forecast(int noutput_items, gr_vector_int& ninput_items_required) override;

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;
};

} /* namespace trellis */
} /* namespace gr */

#endif /* PCCC_DECODER_COMBINED_BLK_IMPL_H */
