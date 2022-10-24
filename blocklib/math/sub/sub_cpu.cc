/* -*- c++ -*- */
/*
 * Copyright 2022 Block Author
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "sub_cpu.h"
#include "sub_cpu_gen.h"

namespace gr {
namespace math {

template <class T>
sub_cpu<T>::sub_cpu(const typename sub<T>::block_args& args)
    : INHERITED_CONSTRUCTORS(T), d_vlen(args.vlen)
{
}

template <class T>
work_return_t sub_cpu<T>::work(work_io& wio)
{
    auto optr = wio.outputs()[0].items<T>();
    int ninputs = wio.inputs().size();

    auto noutput_items = wio.outputs()[0].n_items;

    for (size_t i = 0; i < noutput_items * d_vlen; i++) {
        auto acc = wio.inputs()[0].items<T>()[i];
        for (int j = 1; j < ninputs; j++) {
            auto ptr = wio.inputs()[j].items<T>();
            acc -= ptr[i];
        }

        *optr++ = acc;
    }

    wio.produce_each(noutput_items);
    return work_return_t::OK;
}

} /* namespace math */
} /* namespace gr */
