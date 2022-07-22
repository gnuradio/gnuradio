/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "conjugate_cpu.h"
#include "conjugate_cpu_gen.h"
#include <volk/volk.h>

namespace gr {
namespace math {

conjugate_cpu::conjugate_cpu(const block_args& args) : INHERITED_CONSTRUCTORS
{
    // const int alignment_multiple = volk_get_alignment() / sizeof(gr_complex);
    // set_output_multiple(std::max(1, alignment_multiple));
}

work_return_code_t conjugate_cpu::work(work_io& wio)
{
    auto noutput_items = wio.outputs()[0].n_items;

    auto iptr = wio.inputs()[0].items<gr_complex>();
    auto optr = wio.outputs()[0].items<gr_complex>();

    volk_32fc_conjugate_32fc(optr, iptr, noutput_items);

    wio.produce_each(noutput_items);
    return work_return_code_t::WORK_OK;
}


} // namespace math
} // namespace gr
