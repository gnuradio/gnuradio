/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_METRIC_TYPE_H
#define INCLUDED_DIGITAL_METRIC_TYPE_H

namespace gr {
namespace digital {

enum trellis_metric_type_t {
    TRELLIS_EUCLIDEAN = 200,
    TRELLIS_HARD_SYMBOL,
    TRELLIS_HARD_BIT
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_METRIC_TYPE_H */
