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
    int64_t d_count;  //!< Reset the LFSR after this many bytes (not bits)
    uint64_t d_bytes; //!< Count the processed bytes
    uint8_t d_len;
    uint64_t d_seed;
    uint8_t d_bits_per_byte;
    pmt::pmt_t d_reset_tag_key; //!< Reset the LFSR when this tag is received

    int64_t _get_next_reset_index(int64_t noutput_items, int64_t last_reset_index = -1);

public:
    additive_scrambler_bb_impl(uint64_t mask,
                               uint64_t seed,
                               uint8_t len,
                               int64_t count = 0,
                               uint8_t bits_per_byte = 1,
                               const std::string& reset_tag_key = "");
    ~additive_scrambler_bb_impl() override;

    uint64_t mask() const override;
    uint64_t seed() const override;
    uint8_t len() const override;
    int64_t count() const override;
    uint8_t bits_per_byte() override { return d_bits_per_byte; };

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_ADDITIVE_SCRAMBLER_BB_IMPL_H */
