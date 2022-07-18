/* -*- c++ -*- */
/*
 * Copyright 2022 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "iir_filter_cpu.h"
#include "iir_filter_cpu_gen.h"

namespace gr {
namespace filter {

template <class T_IN, class T_OUT, class TAP_T>
iir_filter_cpu<T_IN, T_OUT, TAP_T>::iir_filter_cpu(
    const typename iir_filter<T_IN, T_OUT, TAP_T>::block_args& args)
    : INHERITED_CONSTRUCTORS(T_IN, T_OUT, TAP_T),
      d_iir(args.fftaps, args.fbtaps, args.oldstyle)
{
}

template <class T_IN, class T_OUT, class TAP_T>
work_return_code_t iir_filter_cpu<T_IN, T_OUT, TAP_T>::work(work_io& wio)
{
    auto in = wio.inputs()[0].items<T_IN>();
    auto out = wio.outputs()[0].items<T_OUT>();
    auto noutput_items = wio.outputs()[0].n_items;

    d_iir.filter_n(out, in, noutput_items);
    wio.produce_each(noutput_items);
    return work_return_code_t::WORK_OK;
}

} /* namespace filter */
} /* namespace gr */
