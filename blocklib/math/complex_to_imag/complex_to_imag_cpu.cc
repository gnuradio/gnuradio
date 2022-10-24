/* -*- c++ -*- */
/*
 * Copyright 2022 Block Author
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "complex_to_imag_cpu.h"
#include "complex_to_imag_cpu_gen.h"
#include <volk/volk.h>

namespace gr {
namespace math {

complex_to_imag_cpu::complex_to_imag_cpu(block_args args)
    : INHERITED_CONSTRUCTORS, d_vlen(args.vlen)
{
}

work_return_t complex_to_imag_cpu::work(work_io& wio)
{
    auto in = wio.inputs()[0].items<gr_complex>();
    auto out = wio.outputs()[0].items<float>();
    auto noutput_items = wio.outputs()[0].n_items;
    int noi = noutput_items * d_vlen;

    volk_32fc_deinterleave_imag_32f(out, in, noi);

    wio.produce_each(noutput_items);
    return work_return_t::OK;
}


} // namespace math
} // namespace gr