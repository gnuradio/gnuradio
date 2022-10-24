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

#include <gnuradio/math/complex_to_imag.h>

namespace gr {
namespace math {

class complex_to_imag_cpu : public virtual complex_to_imag
{
public:
    complex_to_imag_cpu(block_args args);
    work_return_t work(work_io& wio) override;

private:
    const size_t d_vlen;
};

} // namespace math
} // namespace gr