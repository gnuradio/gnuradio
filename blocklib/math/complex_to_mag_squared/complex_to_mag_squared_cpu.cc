/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "complex_to_mag_squared_cpu.h"
#include "complex_to_mag_squared_cpu_gen.h"
#include <volk/volk.h>

namespace gr {
namespace math {
complex_to_mag_squared_cpu::complex_to_mag_squared_cpu(const block_args& args)
    : INHERITED_CONSTRUCTORS, d_vlen(args.vlen)
{
    // const int alignment_multiple = volk_get_alignment() / sizeof(float);
    // set_output_multiple(std::max(1, alignment_multiple));
}

work_return_code_t complex_to_mag_squared_cpu::work(work_io& wio)
{
    auto noutput_items = wio.outputs()[0].n_items;
    int noi = noutput_items * d_vlen;

    auto iptr = wio.inputs()[0].items<gr_complex>();
    auto optr = wio.outputs()[0].items<float>();

    volk_32fc_magnitude_squared_32f(optr, iptr, noi);

    wio.produce_each(noutput_items);
    return work_return_code_t::WORK_OK;
}


} // namespace math
} // namespace gr
