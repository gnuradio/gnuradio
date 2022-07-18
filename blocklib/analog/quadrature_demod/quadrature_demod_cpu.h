/* -*- c++ -*- */
/*
 * Copyright 2004,2012 Free Software Foundation, Inc.
 * Copyright 2022 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/analog/quadrature_demod.h>

namespace gr {
namespace analog {

class quadrature_demod_cpu : public virtual quadrature_demod
{
public:
    quadrature_demod_cpu(block_args args);
    work_return_code_t work(work_io&) override;

private:
    size_t d_history = 2;
};

} // namespace analog
} // namespace gr