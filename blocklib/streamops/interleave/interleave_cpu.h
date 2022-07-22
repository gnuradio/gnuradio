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

#include <gnuradio/streamops/interleave.h>

namespace gr {
namespace streamops {

class interleave_cpu : public virtual interleave
{
public:
    interleave_cpu(block_args args);
    work_return_code_t work(work_io&) override;

private:
    const unsigned int d_ninputs;
    const unsigned int d_blocksize;
    size_t d_itemsize;
};

} // namespace streamops
} // namespace gr