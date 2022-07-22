/* -*- c++ -*- */
/*
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "add_cuda.h"
#include "add_cuda_gen.h"
#include <volk/volk.h>

namespace gr {
namespace math {

template <class T>
add_cuda<T>::add_cuda(const typename add<T>::block_args& args)
    : INHERITED_CONSTRUCTORS(T), d_vlen(args.vlen), d_nports(args.nports)
{
    d_in_items.resize(d_nports);
    p_add_kernel = std::make_shared<cusp::add<T>>(d_nports);
}

template <class T>
work_return_code_t add_cuda<T>::work(work_io& wio)
{
    auto out = wio.outputs()[0].items<T>();
    auto noutput_items = wio.outputs()[0].n_items;
    int noi = d_vlen * noutput_items;

    size_t idx = 0;
    for (auto& wi : wio.inputs()) {
        d_in_items[idx++] = wi.items<T>();
    }

    p_add_kernel->launch_default_occupancy(d_in_items, { out }, noi);

    wio.produce_each(noutput_items);
    wio.consume_each(noutput_items);
    return work_return_code_t::WORK_OK;
}

} /* namespace math */
} /* namespace gr */
