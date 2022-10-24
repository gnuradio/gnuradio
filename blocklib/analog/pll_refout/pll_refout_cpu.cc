/* -*- c++ -*- */
/*
 * Copyright 2022 Block Author
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "pll_refout_cpu.h"
#include "pll_refout_cpu_gen.h"

namespace gr {
namespace analog {

pll_refout_cpu::pll_refout_cpu(block_args args)
    : sync_block("pll_refout", "analog"),
      control_loop(args.loop_bw, args.max_freq, args.min_freq),
      pll_refout(args)
{
}

work_return_t pll_refout_cpu::work(work_io& wio)
{
    auto iptr = wio.inputs()[0].items<gr_complex>();
    auto optr = wio.outputs()[0].items<gr_complex>();

    float error;
    float t_imag, t_real;
    int size = wio.outputs()[0].n_items;

    while (size-- > 0) {
        sincosf(d_phase, &t_imag, &t_real);
        *optr++ = gr_complex(t_real, t_imag);

        error = phase_detector(*iptr++, d_phase);

        advance_loop(error);
        phase_wrap();
        frequency_limit();
    }

    wio.produce_each(wio.outputs()[0].n_items);
    return work_return_t::OK;
}


} // namespace analog
} // namespace gr