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

#include <gnuradio/streamops/keep_m_in_n.h>

namespace gr {
namespace streamops {

class keep_m_in_n_cpu : public virtual keep_m_in_n
{
public:
    keep_m_in_n_cpu(block_args args);
    work_return_code_t work(work_io&) override;

private:
    void on_parameter_change(param_action_sptr action) override;
};

} // namespace streamops
} // namespace gr