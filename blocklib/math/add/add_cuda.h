/* -*- c++ -*- */
/*
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/math/add.h>
#include <cusp/add.cuh>

namespace gr {
namespace math {

template <class T>
class add_cuda : public add<T>
{
public:
    add_cuda(const typename add<T>::block_args& args);

    work_return_code_t work(work_io&) override;

private:
    const size_t d_vlen;
    const size_t d_nports;
    std::vector<const void*> d_in_items;

    std::shared_ptr<cusp::add<T>> p_add_kernel;
};


} // namespace math
} // namespace gr
