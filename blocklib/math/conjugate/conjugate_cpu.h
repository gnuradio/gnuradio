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

#include <gnuradio/math/conjugate.h>
#include <volk/volk.h>

namespace gr {
namespace math {

class conjugate_cpu : public conjugate
{
public:
    conjugate_cpu(const block_args& args);

    work_return_code_t work(work_io&) override;
};

} // namespace math
} // namespace gr