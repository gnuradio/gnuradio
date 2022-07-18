/* -*- c++ -*- */
/*
 * Copyright 2022 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/analog/noise_source.h>
#include <gnuradio/kernel/math/random.h>

namespace gr {
namespace analog {

template <class T>
class noise_source_cpu : public noise_source<T>
{
public:
    noise_source_cpu(const typename noise_source<T>::block_args& args);

    work_return_code_t work(work_io&) override;

private:
    kernel::math::random d_rng;
};


} // namespace analog
} // namespace gr
