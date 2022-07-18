/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/math/complex_to_mag.h>
#include <volk/volk.h>
namespace gr {
namespace math {

class complex_to_mag_cpu : public complex_to_mag
{
public:
    complex_to_mag_cpu(const block_args& args);
    work_return_code_t work(work_io&) override;

private:
    size_t d_vlen;
};

} // namespace math
} // namespace gr