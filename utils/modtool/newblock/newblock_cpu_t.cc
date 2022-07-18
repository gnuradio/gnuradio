/* -*- c++ -*- */
/*
 * Copyright <COPYRIGHT_YEAR> <COPYRIGHT_AUTHOR>
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "newblock_cpu.h"
#include "newblock_cpu_gen.h"

namespace gr {
namespace newmod {

template <class T>
newblock_cpu<T>::newblock_cpu(const typename newblock<T>::block_args& args)
    : INHERITED_CONSTRUCTORS(T)
{
}

template <class T>
work_return_code_t newblock_cpu<T>::work(work_io&)
{
    // Do work specific code here
    return work_return_code_t::WORK_OK;
}

} /* namespace newmod */
} /* namespace gr */
