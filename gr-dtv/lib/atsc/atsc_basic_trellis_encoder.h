/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ATSC_TRELLIS_ENCODER_H
#define INCLUDED_ATSC_TRELLIS_ENCODER_H

/*!
 * \brief ATSC trellis encoder building block.
 *
 * Note this is NOT the 12x interleaved interface.
 *
 * This implements a single instance of the ATSC trellis encoder.
 * This is a rate 2/3 encoder (really a constraint length 3, rate 1/2
 * encoder with the top bit passed through unencoded. This does not
 * implement the "precoding" of the top bit, because the NTSC rejection
 * filter is not supported.
 */

namespace gr {
namespace dtv {

class atsc_basic_trellis_encoder
{
private:
    int state; // two bit state;

public:
    atsc_basic_trellis_encoder() : state(0) {}

    /*!
     * Encode two bit INPUT into 3 bit return value. Domain is [0,3],
     * Range is [0,7]. The mapping to bipolar levels is not done.
     */
    int encode(unsigned int input);

    //! reset encoder state
    void reset() { state = 0; }

    static const unsigned char next_state[32];
    static const unsigned char out_symbol[32];
};

} /* namespace dtv */
} /* namespace gr */

#endif /* INCLUDED_ATSC_TRELLIS_ENCODER_H */
