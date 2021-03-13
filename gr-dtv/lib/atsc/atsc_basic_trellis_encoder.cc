/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "atsc_basic_trellis_encoder.h"
#include <cassert>

namespace gr {
namespace dtv {

const unsigned char atsc_basic_trellis_encoder::next_state[32] = {
    0, 1, 4, 5, 2, 3, 6, 7, 1, 0, 5, 4, 3, 2, 7, 6,
    4, 5, 0, 1, 6, 7, 2, 3, 5, 4, 1, 0, 7, 6, 3, 2
};

const unsigned char atsc_basic_trellis_encoder::out_symbol[32] = {
    0, 2, 4, 6, 1, 3, 5, 7, 0, 2, 4, 6, 1, 3, 5, 7,
    4, 6, 0, 2, 5, 7, 1, 3, 4, 6, 0, 2, 5, 7, 1, 3
};

/*!
 * Encode two bit INPUT into 3 bit return value. Domain is [0,3],
 * Range is [0,7]. The mapping to bipolar levels is not done.
 */

int atsc_basic_trellis_encoder::encode(unsigned int input)
{
    assert(input < 4);
    int index = (state << 2) + input;
    state = next_state[index];
    return out_symbol[index];
}

} /* namespace dtv */
} /* namespace gr */
