/* -*- c++ -*- */
/*
 * Copyright 2006,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_TEST_TYPES_H
#define INCLUDED_GR_TEST_TYPES_H

namespace gr {

typedef enum {
    CONSUME_NOUTPUT_ITEMS = 0,
    CONSUME_NOUTPUT_ITEMS_LIMIT_MAX = 1,
    CONSUME_NOUTPUT_ITEMS_LIMIT_MIN = 2,
    CONSUME_ALL_AVAILABLE = 3,
    CONSUME_ALL_AVAILABLE_LIMIT_MAX = 4,
    /*CONSUME_ALL_AVAILABLE_LIMIT_MIN=5,*/
    CONSUME_ZERO = 6,
    CONSUME_ONE = 7,
    CONSUME_MINUS_ONE = 8
} consume_type_t;

typedef enum {
    PRODUCE_NOUTPUT_ITEMS = 0,
    PRODUCE_NOUTPUT_ITEMS_LIMIT_MAX = 1,
    /*PRODUCE_NOUTPUT_ITEMS_LIMIT_MIN=2,*/
    PRODUCE_ZERO = 6,
    PRODUCE_ONE = 7,
    PRODUCE_MINUS_ONE = 8
} produce_type_t;

} /* namespace gr */

#endif /* INCLUDED_GR_TEST_TYPES_H */
