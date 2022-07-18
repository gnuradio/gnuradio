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

#include <gnuradio/streamops/interleaved_short_to_complex.h>

namespace gr {
namespace streamops {

class interleaved_short_to_complex_cpu : public interleaved_short_to_complex
{
public:
    interleaved_short_to_complex_cpu(const block_args& args);

    work_return_code_t work(work_io&) override;


    void set_swap(bool swap);
    void set_scale_factor(float new_value) override;

private:
    float d_scalar;
    bool d_swap;
};


} // namespace streamops
} // namespace gr
