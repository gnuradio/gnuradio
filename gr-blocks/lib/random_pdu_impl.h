/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_RANDOM_PDU_IMPL_H
#define INCLUDED_BLOCKS_RANDOM_PDU_IMPL_H

#include <gnuradio/blocks/random_pdu.h>
#include <boost/generator_iterator.hpp>

#include <random>

namespace gr {
namespace blocks {

class random_pdu_impl : public random_pdu
{
private:
    std::mt19937 d_rng;
    std::uniform_int_distribution<> d_urange;
    std::uniform_int_distribution<> d_brange;
    unsigned char d_mask;
    int d_length_modulo;

public:
    random_pdu_impl(int min_items,
                    int max_items,
                    unsigned char byte_mask,
                    int length_modulo);

    bool start();
    void output_random();
    void generate_pdu(pmt::pmt_t msg) { output_random(); }
    void generate_pdu() { output_random(); }
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_RANDOM_PDU_IMPL_H */
