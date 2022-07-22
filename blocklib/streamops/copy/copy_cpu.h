/* -*- c++ -*- */
/*
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/streamops/copy.h>

namespace gr {
namespace streamops {

class copy_cpu : public copy
{
public:
    copy_cpu(block_args args);
    work_return_code_t work(work_io&) override;
};

} // namespace streamops
} // namespace gr