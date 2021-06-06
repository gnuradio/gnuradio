/* -*- c++ -*- */
/*
 * Copyright (C) 2021 Daniel Estevez <daniel@destevez.net>
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_DIFF_CODING_TYPE_H
#define INCLUDED_DIGITAL_DIFF_CODING_TYPE_H

namespace gr {
namespace digital {

// Differential coding types
enum diff_coding_type {
    DIFF_DIFFERENTIAL = 0, // Standard differential coding x[n] - x[n-1] % M
    DIFF_NRZI = 1,         // NRZI coding
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_DIFF_CODING_TYPE_H */
