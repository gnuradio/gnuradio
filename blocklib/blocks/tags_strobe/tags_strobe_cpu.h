/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/blocks/tags_strobe.h>

namespace gr {
namespace blocks {

class tags_strobe_cpu : public virtual tags_strobe
{
public:
    tags_strobe_cpu(block_args args);
    work_return_code_t work(work_io& wio) override;

private:
    uint64_t d_nsamps;
    tag_t d_tag;
    uint64_t d_offset = 0;
};

} // namespace blocks
} // namespace gr