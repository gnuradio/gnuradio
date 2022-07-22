/* -*- c++ -*- */
/*
 * Copyright 2007,2012-2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/streamops/delay.h>
#include <mutex>

namespace gr {
namespace streamops {

class delay_cpu : public delay
{
public:
    delay_cpu(const block_args& args);
    work_return_code_t work(work_io&) override;
    size_t dly() override { return d_delay; }
    void set_dly(size_t d) override;

protected:
    size_t d_delay = 0;
    int d_delta = 0;


    void on_parameter_change(param_action_sptr action) override
    {
        // This will set the underlying PMT
        block::on_parameter_change(action);

        // Do more updating for certain parameters
        if (action->id() == delay::id_dly) {
            auto dly = pmtf::get_as<double>(*this->param_dly);
            set_dly(dly);
        }
    }
};

} // namespace streamops
} // namespace gr