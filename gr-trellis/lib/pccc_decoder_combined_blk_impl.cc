/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "pccc_decoder_combined_blk_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/trellis/core_algorithms.h>
#include <iostream>

namespace gr {
namespace trellis {

template <class IN_T, class OUT_T>
typename pccc_decoder_combined_blk<IN_T, OUT_T>::sptr
pccc_decoder_combined_blk<IN_T, OUT_T>::make(const fsm& FSMo,
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
                                             float scaling)
{
    return gnuradio::make_block_sptr<pccc_decoder_combined_blk_impl<IN_T, OUT_T>>(
        FSMo,
        STo0,
        SToK,
        FSMi,
        STi0,
        STiK,
        INTERLEAVER,
        blocklength,
        repetitions,
        SISO_TYPE,
        D,
        TABLE,
        METRIC_TYPE,
        scaling);
}

template <class IN_T, class OUT_T>
pccc_decoder_combined_blk_impl<IN_T, OUT_T>::pccc_decoder_combined_blk_impl(
    const fsm& FSMo,
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
    float scaling)
    : block("pccc_decoder_combined_blk<IN_T,OUT_T>",
            io_signature::make(1, 1, sizeof(IN_T)),
            io_signature::make(1, 1, sizeof(OUT_T))),
      d_FSMo(FSMo),
      d_STo0(STo0),
      d_SToK(SToK),
      d_FSMi(FSMi),
      d_STi0(STi0),
      d_STiK(STiK),
      d_INTERLEAVER(INTERLEAVER),
      d_blocklength(blocklength),
      d_repetitions(repetitions),
      d_SISO_TYPE(SISO_TYPE),
      d_D(D),
      d_TABLE(TABLE),
      d_METRIC_TYPE(METRIC_TYPE),
      d_scaling(scaling)
{
    assert(d_FSMo.I() == d_FSMi.I());
    this->set_relative_rate(1, (uint64_t)d_D);
    this->set_output_multiple(d_blocklength);
}

template <class IN_T, class OUT_T>
pccc_decoder_combined_blk_impl<IN_T, OUT_T>::~pccc_decoder_combined_blk_impl()
{
}

template <class IN_T, class OUT_T>
void pccc_decoder_combined_blk_impl<IN_T, OUT_T>::set_scaling(float scaling)
{
    gr::thread::scoped_lock guard(this->d_setlock);
    d_scaling = scaling;
}

template <class IN_T, class OUT_T>
void pccc_decoder_combined_blk_impl<IN_T, OUT_T>::forecast(
    int noutput_items, gr_vector_int& ninput_items_required)
{
    int input_required = d_D * noutput_items;
    ninput_items_required[0] = input_required;
}

//===========================================================

template <class IN_T, class OUT_T>
int pccc_decoder_combined_blk_impl<IN_T, OUT_T>::general_work(
    int noutput_items,
    gr_vector_int& ninput_items,
    gr_vector_const_void_star& input_items,
    gr_vector_void_star& output_items)
{
    gr::thread::scoped_lock guard(this->d_setlock);
    int nblocks = noutput_items / d_blocklength;

    float (*p2min)(float, float) = NULL;
    if (d_SISO_TYPE == TRELLIS_MIN_SUM)
        p2min = &min;
    else if (d_SISO_TYPE == TRELLIS_SUM_PRODUCT)
        p2min = &min_star;

    const IN_T* in = (const IN_T*)input_items[0];
    OUT_T* out = (OUT_T*)output_items[0];
    for (int n = 0; n < nblocks; n++) {
        pccc_decoder_combined(d_FSMo,
                              d_STo0,
                              d_SToK,
                              d_FSMi,
                              d_STi0,
                              d_STiK,
                              d_INTERLEAVER,
                              d_blocklength,
                              d_repetitions,
                              p2min,
                              d_D,
                              d_TABLE,
                              d_METRIC_TYPE,
                              d_scaling,
                              &(in[n * d_blocklength * d_D]),
                              &(out[n * d_blocklength]));
    }

    this->consume_each(d_D * noutput_items);
    return noutput_items;
}
template class pccc_decoder_combined_blk<float, std::uint8_t>;
template class pccc_decoder_combined_blk<float, std::int16_t>;
template class pccc_decoder_combined_blk<float, std::int32_t>;
template class pccc_decoder_combined_blk<gr_complex, std::uint8_t>;
template class pccc_decoder_combined_blk<gr_complex, std::int16_t>;
template class pccc_decoder_combined_blk<gr_complex, std::int32_t>;


} /* namespace trellis */
} /* namespace gr */
