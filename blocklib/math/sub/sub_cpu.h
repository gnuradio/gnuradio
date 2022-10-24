/* -*- c++ -*- */
/*
 * Copyright 2022 Block Author
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/math/sub.h>

namespace gr {
namespace math {

template <class T>
class sub_cpu : public sub<T>
{
public:
    sub_cpu(const typename sub<T>::block_args& args);

    work_return_t work(work_io& wio) override;

private:
    size_t d_vlen;
};


} // namespace math
} // namespace gr
