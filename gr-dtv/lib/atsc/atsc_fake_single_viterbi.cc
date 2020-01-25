/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "atsc_fake_single_viterbi.h"

namespace gr {
namespace dtv {

void atsc_fake_single_viterbi::reset() { post_coder_state = 0; }

atsc_fake_single_viterbi::atsc_fake_single_viterbi() { reset(); }

/*
 * implement simple slicer and post coder
 */
char atsc_fake_single_viterbi::decode(float input)
{
    int y2, y1;

    if (input < -4) {
        y2 = 0;
        y1 = 0;
    } else if (input < 0) {
        y2 = 0;
        y1 = 1;
    } else if (input < 4) {
        y2 = 1;
        y1 = 0;
    } else {
        y2 = 1;
        y1 = 1;
    }

    int x1 = y1;
    int x2 = y2 ^ post_coder_state;
    post_coder_state = y2;

    return (x2 << 1) | x1;
}

} /* namespace dtv */
} /* namespace gr */
