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

#include <gnuradio/streamops/selector.h>

namespace gr {
namespace streamops {

class selector_cpu : public virtual selector
{
public:
    selector_cpu(block_args args);
    work_return_code_t work(work_io&) override;

private:
    size_t d_itemsize = 0;
    size_t d_num_inputs, d_num_outputs;
};

} // namespace streamops
} // namespace gr