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

#include <gnuradio/analog/wfm_rcv_pll.h>

namespace gr {
namespace analog {

class wfm_rcv_pll_cpu : public virtual wfm_rcv_pll
{
public:
    wfm_rcv_pll_cpu(block_args args);
};

} // namespace analog
} // namespace gr