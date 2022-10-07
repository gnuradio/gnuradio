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

#include <gnuradio/streamops/size_adapter.h>

namespace gr {
namespace streamops {

class size_adapter_cpu : public virtual size_adapter
{
public:
    size_adapter_cpu(block_args args);
    work_return_t work(work_io& wio) override;

private:
    size_t _itemsize_in = 0;
    size_t _itemsize_out = 0;
};

} // namespace streamops
} // namespace gr