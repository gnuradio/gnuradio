/* -*- c++ -*- */
/*
 * Copyright (C) 2017 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_TIMING_ERROR_DETECTOR_TYPE_H
#define INCLUDED_DIGITAL_TIMING_ERROR_DETECTOR_TYPE_H

namespace gr {
namespace digital {

// Timing Error Detector types.
enum ted_type {
    TED_NONE = -1,
    TED_MUELLER_AND_MULLER = 0,     // Decision directed
    TED_MOD_MUELLER_AND_MULLER = 1, // Decision directed
    TED_ZERO_CROSSING = 2,          // Decision directed
    TED_GARDNER = 4,
    TED_EARLY_LATE = 5,
    TED_DANDREA_AND_MENGALI_GEN_MSK = 6, // Operates on the CPM signal
    TED_SIGNAL_TIMES_SLOPE_ML = 7,       // ML approx. for small signal
    TED_SIGNUM_TIMES_SLOPE_ML = 8,       // ML approx. for large signal
    TED_MENGALI_AND_DANDREA_GMSK = 9,    // Operates on the CPM signal
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_TIMING_ERROR_DETECTOR_TYPE_H */
