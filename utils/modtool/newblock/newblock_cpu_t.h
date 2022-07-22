/* -*- c++ -*- */
/*
 * Copyright <COPYRIGHT_YEAR> <COPYRIGHT_AUTHOR>
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/newmod/newblock.h>

namespace gr {
namespace newmod {

template <class T>
class newblock_cpu : public newblock<T>
{
public:
    newblock_cpu(const typename newblock<T>::block_args& args);

    work_return_code_t work(work_io&) override;

private:
    // Declare private variables here
};


} // namespace newmod
} // namespace gr
