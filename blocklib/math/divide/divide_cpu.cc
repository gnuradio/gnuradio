/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2010,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "divide_cpu.h"
#include "divide_cpu_gen.h"
#include <volk/volk.h>

namespace gr {
namespace math {

template <class T>
divide_cpu<T>::divide_cpu(const typename divide<T>::block_args& args)
    : INHERITED_CONSTRUCTORS(T), d_num_inputs(args.num_inputs), d_vlen(args.vlen)
{
}

template <>
divide_cpu<float>::divide_cpu(const typename divide<float>::block_args& args)
    : INHERITED_CONSTRUCTORS(float), d_num_inputs(args.num_inputs), d_vlen(args.vlen)
{
    // const int alignment_multiple = volk_get_alignment() / sizeof(float);
    // set_output_multiple(std::max(1, alignment_multiple));
}

template <>
divide_cpu<gr_complex>::divide_cpu(const typename divide<gr_complex>::block_args& args)
    : INHERITED_CONSTRUCTORS(gr_complex), d_num_inputs(args.num_inputs), d_vlen(args.vlen)
{
    const int alignment_multiple = volk_get_alignment() / sizeof(gr_complex);
    set_output_multiple(std::max(1, alignment_multiple));
}


template <>
work_return_code_t divide_cpu<float>::work(work_io& wio)
{
    auto optr = wio.outputs()[0].items<float>();
    auto noutput_items = wio.outputs()[0].n_items;

    auto numerator = wio.inputs()[0].items<float>();
    for (size_t inp = 1; inp < d_num_inputs; ++inp) {
        volk_32f_x2_divide_32f(
            optr, numerator, wio.inputs()[inp].items<float>(), noutput_items * d_vlen);
        numerator = optr;
    }

    wio.outputs()[0].n_produced = wio.outputs()[0].n_items;
    return work_return_code_t::WORK_OK;
}

template <>
work_return_code_t divide_cpu<gr_complex>::work(work_io& wio)

{
    auto optr = wio.outputs()[0].items<gr_complex>();
    auto noutput_items = wio.outputs()[0].n_items;

    auto numerator = wio.inputs()[0].items<gr_complex>();
    for (size_t inp = 1; inp < d_num_inputs; ++inp) {
        volk_32fc_x2_divide_32fc(optr,
                                 numerator,
                                 wio.inputs()[inp].items<gr_complex>(),
                                 noutput_items * d_vlen);
        numerator = optr;
    }

    wio.outputs()[0].n_produced = wio.outputs()[0].n_items;
    return work_return_code_t::WORK_OK;
}

template <class T>
work_return_code_t divide_cpu<T>::work(work_io& wio)

{
    auto optr = wio.outputs()[0].items<T>();
    auto noutput_items = wio.outputs()[0].n_items;

    for (size_t i = 0; i < noutput_items * d_vlen; i++) {
        T acc = (wio.inputs()[0].items<T>())[i];
        for (size_t j = 1; j < d_num_inputs; j++) {
            acc /= (wio.inputs()[j].items<T>())[i];
        }
        *optr++ = static_cast<T>(acc);
    }

    wio.outputs()[0].n_produced = wio.outputs()[0].n_items;
    wio.inputs()[0].n_consumed = wio.inputs()[0].n_items;
    return work_return_code_t::WORK_OK;
}


} /* namespace math */
} /* namespace gr */
