/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2010,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/math/multiply.h>

namespace gr {
namespace math {

template <class T>
class multiply_cpu : public multiply<T>
{
public:
    multiply_cpu(const typename multiply<T>::block_args& args);

    work_return_code_t work(work_io&) override;

protected:
    size_t d_num_inputs;
    size_t d_vlen;
};


} // namespace math
} // namespace gr
