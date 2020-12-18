/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ATSC_RANDOMIZE_H
#define INCLUDED_ATSC_RANDOMIZE_H

#include "atsc_types.h"
#include <stdint.h>

namespace gr {
namespace dtv {

class atsc_randomize
{
public:
    atsc_randomize();

    /*! \brief reset randomizer LFSR
     *
     * must be called during the Data Segment Sync interval prior to the
     * first data segment.  I.e., the LFSR is reset prior to the first
     * field of each VSB data frame.
     */
    void reset();

    //! randomize (whiten) mpeg packet and remove leading MPEG-2 sync byte
    void randomize(atsc_mpeg_packet_no_sync& out, const atsc_mpeg_packet& in);

    //! derandomize (de-whiten) mpeg packet and add leading MPEG-2 sync byte
    void derandomize(uint8_t* out, const uint8_t* in);

    unsigned int state() const { return d_state; }

private:
    static void initialize_output_map();
    static unsigned char slow_output_map(int st);

    static unsigned char fast_output_map(int st)
    {
        return s_output_map[(st & 0xb23c) >>
                            2]; // Magic const with 8 bits set improves cache
                                // utilization.  The bits correspond to the taps
                                // used in output calculation.  Others may be
                                // safely ignored.
    }

    //! return current output value
    unsigned char output() { return fast_output_map(d_state); }

    //! clock LFSR; advance to next state.
    void clk()
    {
        if (d_state & 0x1)
            d_state = ((d_state ^ MASK) >> 1) | 0x8000;
        else
            d_state = d_state >> 1;
    }

    //! return current output value and advance to next state
    unsigned char output_and_clk()
    {
        unsigned char r = output();
        clk();
        return r;
    }

    unsigned int d_state;

    static constexpr unsigned int PRELOAD_VALUE = 0x018f; /* 0xf180 bit reversed */
    static constexpr unsigned int MASK = 0xa638;
    static unsigned char s_output_map[1 << 14];
    static bool s_output_map_initialized_p;
};
} /* namespace dtv */
} /* namespace gr */

#endif /* INCLUDED_ATSC_RANDOMIZE_H */
