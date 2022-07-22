/* -*- c++ -*- */
/*
 * Copyright 2022 FIXME
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/analog/fm_deemph.h>
#include <gnuradio/filter/iir_filter.h>

namespace gr {
namespace analog {

class fm_deemph_cpu : public virtual fm_deemph
{
public:
    fm_deemph_cpu(block_args args);

private:
    filter::iir_filter_ffd::sptr deemph;
};

} // namespace analog
} // namespace gr