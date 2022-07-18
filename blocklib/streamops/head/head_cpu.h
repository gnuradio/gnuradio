/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2012,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/streamops/head.h>

namespace gr {
namespace streamops {

class head_cpu : public head
{
public:
    head_cpu(const block_args& args);
    work_return_code_t work(work_io&) override;

private:
    size_t d_nitems;
    size_t d_ncopied_items = 0;
};

} // namespace streamops
} // namespace gr