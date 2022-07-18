/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2010,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "vector_sink_cpu.h"
#include "vector_sink_cpu_gen.h"
#include <volk/volk.h>

namespace gr {
namespace blocks {

template <class T>
vector_sink_cpu<T>::vector_sink_cpu(const typename vector_sink<T>::block_args& args)
    : INHERITED_CONSTRUCTORS(T), d_vlen(args.vlen)
{
    d_data.reserve(d_vlen * args.reserve_items);
}

template <class T>
work_return_code_t vector_sink_cpu<T>::work(work_io& wio)
{
    auto iptr = wio.inputs()[0].items<T>();
    int noutput_items = wio.inputs()[0].n_items;

    for (unsigned int i = 0; i < noutput_items * d_vlen; i++)
        d_data.push_back(iptr[i]);

    auto tags = wio.inputs()[0].tags_in_window(0, noutput_items);
    d_tags.insert(d_tags.end(), tags.begin(), tags.end());

    wio.consume_each(noutput_items);
    this->d_debug_logger->debug("sizeof_data: {}", d_data.size());
    return work_return_code_t::WORK_OK;
}

} /* namespace blocks */
} /* namespace gr */
