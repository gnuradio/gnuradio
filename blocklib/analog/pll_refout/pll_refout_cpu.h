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

#include <gnuradio/analog/pll_refout.h>

using namespace gr::kernel::math;

namespace gr {
namespace analog {

class pll_refout_cpu : public virtual pll_refout
{
public:
    pll_refout_cpu(block_args args);
    work_return_t work(work_io& wio) override;

private:
    float mod_2pi(float in)
    {
        if (in > GR_M_PI)
            return in - (2.0 * GR_M_PI);
        else if (in < -GR_M_PI)
            return in + (2.0 * GR_M_PI);
        else
            return in;
    }

    float phase_detector(gr_complex sample, float ref_phase)
    {
        float sample_phase;
        sample_phase = fast_atan2f(sample.imag(), sample.real());
        return mod_2pi(sample_phase - ref_phase);
    }
};

} // namespace analog
} // namespace gr