/* -*- c++ -*- */
/*
 * Copyright 2008,2010,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_ADDITIVE_SCRAMBLER_BB_IMPL_H
#define INCLUDED_DIGITAL_ADDITIVE_SCRAMBLER_BB_IMPL_H

#include <gnuradio/digital/additive_scrambler_bb.h>
#include <gnuradio/digital/lfsr.h>

namespace gr {
namespace digital {

class additive_scrambler_bb_impl : public additive_scrambler_bb
{
private:
    digital::lfsr d_lfsr;
    int d_count; //!< Reset the LFSR after this many bytes (not bits)
    int d_bytes; //!< Count the processed bytes
    int d_len;
    int d_seed;
    int d_bits_per_byte;
    pmt::pmt_t d_reset_tag_key; //!< Reset the LFSR when this tag is received

    int _get_next_reset_index(int noutput_items, int last_reset_index = -1);

public:
    additive_scrambler_bb_impl(int mask,
                               int seed,
                               int len,
                               int count = 0,
                               int bits_per_byte = 1,
                               const std::string& reset_tag_key = "");
    ~additive_scrambler_bb_impl();

    int mask() const;
    int seed() const;
    int len() const;
    int count() const;
    int bits_per_byte() { return d_bits_per_byte; };

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_ADDITIVE_SCRAMBLER_BB_IMPL_H */
