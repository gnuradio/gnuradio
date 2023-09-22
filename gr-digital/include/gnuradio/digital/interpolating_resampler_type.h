/* -*- c++ -*- */
/*
 * Copyright (C) 2017 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_INTERPOLATING_RESAMPLER_TYPE_H
#define INCLUDED_DIGITAL_INTERPOLATING_RESAMPLER_TYPE_H

namespace gr {
namespace digital {

// Interpolating Resampler type
enum ir_type {
    IR_NONE = -1,
    IR_MMSE_8TAP = 0, // Valid for [-Fs/4, Fs/4] bandlimited input
    IR_PFB_NO_MF = 1, // No matched filtering, just interpolation
    IR_PFB_MF = 2,
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_INTERPOLATING_RESAMPLER_TYPE_H */
