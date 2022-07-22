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

#include <gnuradio/streamops/deinterleave.h>

namespace gr {
namespace streamops {

class deinterleave_cpu : public virtual deinterleave
{
public:
    deinterleave_cpu(block_args args);
    work_return_code_t work(work_io&) override;

private:
    size_t d_current_output = 0;
    size_t d_size_bytes = 0; // block size in bytes
};

} // namespace streamops
} // namespace gr