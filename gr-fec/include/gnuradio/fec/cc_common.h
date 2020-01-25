/* -*- c++ -*- */
/*
 * Copyright 2013-2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FEC_CC_COMMON_H
#define INCLUDED_FEC_CC_COMMON_H

typedef enum _cc_mode_t {
    CC_STREAMING = 0,
    CC_TERMINATED,
    CC_TRUNCATED,
    CC_TAILBITING
} cc_mode_t;

typedef union {
    // decision_t is a BIT vector
    unsigned char* t;
    unsigned int* w;
    unsigned short* s;
    unsigned char* c;
} decision_t;

typedef union {
    unsigned char* t;
} metric_t;

struct v {
    unsigned char* metrics;
    metric_t old_metrics, new_metrics, metrics1,
        metrics2; /* Pointers to path metrics, swapped on every bit */
    unsigned char* decisions;
};

#endif /*INCLUDED_FEC_CC_COMMON_H*/
