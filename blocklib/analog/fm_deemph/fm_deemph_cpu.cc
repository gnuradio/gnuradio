/* -*- c++ -*- */
/*
 * Copyright 2022 FIXME
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "fm_deemph_cpu.h"
#include "fm_deemph_cpu_gen.h"

#include <gnuradio/filter/iir_filter.h>

namespace gr {
namespace analog {

fm_deemph_cpu::fm_deemph_cpu(block_args args) : INHERITED_CONSTRUCTORS
{
    // Digital corner frequency
    auto w_c = 1.0 / args.tau;

    // Prewarped analog corner frequency
    auto w_ca = 2.0 * args.fs * tanf(w_c / (2.0 * args.fs));

    // Resulting digital pole, zero, and gain term from the bilinear
    // transformation of H(s) = w_ca / (s + w_ca) to
    // H(z) = b0 (1 - z1 z^-1)/(1 - p1 z^-1)
    auto k = -w_ca / (2.0 * args.fs);
    auto z1 = -1.0;
    auto p1 = (1.0 + k) / (1.0 - k);
    auto b0 = -k / (1.0 - k);

    std::vector<double> btaps{ b0 * 1.0, b0 * -z1 };
    std::vector<double> ataps{ 1.0, -p1 };

    // Since H(s = 0) = 1.0, then H(z = 1) = 1.0 and has 0 dB gain at DC

    deemph = filter::iir_filter_ffd::make({ btaps, ataps, false });
    hier_block::connect(self(), 0, deemph, 0);
    hier_block::connect(deemph, 0, self(), 0);
}


} // namespace analog
} // namespace gr