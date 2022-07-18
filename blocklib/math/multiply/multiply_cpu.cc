/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2010,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "multiply_cpu.h"
#include "multiply_cpu_gen.h"
#include <volk/volk.h>

namespace gr {
namespace math {

template <class T>
multiply_cpu<T>::multiply_cpu(const typename multiply<T>::block_args& args)
    : INHERITED_CONSTRUCTORS(T), d_num_inputs(args.num_inputs), d_vlen(args.vlen)
{
}

template <>
multiply_cpu<float>::multiply_cpu(const typename multiply<float>::block_args& args)
    : INHERITED_CONSTRUCTORS(float), d_num_inputs(args.num_inputs), d_vlen(args.vlen)
{
    // const int alignment_multiple = volk_get_alignment() / sizeof(float);
    // set_output_multiple(std::max(1, alignment_multiple));
}

template <>
multiply_cpu<gr_complex>::multiply_cpu(
    const typename multiply<gr_complex>::block_args& args)
    : INHERITED_CONSTRUCTORS(gr_complex), d_num_inputs(args.num_inputs), d_vlen(args.vlen)
{
    // const int alignment_multiple = volk_get_alignment() / sizeof(gr_complex);
    // set_output_multiple(std::max(1, alignment_multiple));
}

template <>
work_return_code_t multiply_cpu<float>::work(work_io& wio)
{
    auto out = wio.outputs()[0].items<float>();
    auto noutput_items = wio.outputs()[0].n_items;
    int noi = d_vlen * noutput_items;

    memcpy(out, wio.inputs()[0].items<float>(), noi * sizeof(float));
    for (size_t i = 1; i < d_num_inputs; i++) {
        volk_32f_x2_multiply_32f(out, out, wio.inputs()[i].items<float>(), noi);
    }

    wio.outputs()[0].n_produced = wio.outputs()[0].n_items;
    return work_return_code_t::WORK_OK;
}

template <>
work_return_code_t multiply_cpu<gr_complex>::work(work_io& wio)

{
    auto out = wio.outputs()[0].items<gr_complex>();
    auto noutput_items = wio.outputs()[0].n_items;
    int noi = d_vlen * noutput_items;

    memcpy(out, wio.inputs()[0].items<gr_complex>(), noi * sizeof(gr_complex));
    for (size_t i = 1; i < d_num_inputs; i++) {
        volk_32fc_x2_multiply_32fc(out, out, wio.inputs()[i].items<gr_complex>(), noi);
    }

    wio.outputs()[0].n_produced = wio.outputs()[0].n_items;
    return work_return_code_t::WORK_OK;
}

template <class T>
work_return_code_t multiply_cpu<T>::work(work_io& wio)

{
    auto optr = wio.outputs()[0].items<T>();
    auto noutput_items = wio.outputs()[0].n_items;

    for (size_t i = 0; i < noutput_items * d_vlen; i++) {
        T acc = (wio.inputs()[0].items<T>())[i];
        for (size_t j = 1; j < d_num_inputs; j++) {
            acc *= (wio.inputs()[j].items<T>())[i];
        }
        *optr++ = static_cast<T>(acc);
    }

    wio.outputs()[0].n_produced = wio.outputs()[0].n_items;
    wio.inputs()[0].n_consumed = wio.inputs()[0].n_items;
    return work_return_code_t::WORK_OK;
}

} /* namespace math */
} /* namespace gr */
