/* -*- c++ -*- */
/*
 * Copyright 2021 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/streamops/load.h>

namespace gr {
namespace streamops {

class load_cpu : public load
{
public:
    load_cpu(block_args args);
    work_return_code_t work(work_io&) override;

protected:
    size_t d_load;
};

} // namespace streamops
} // namespace gr