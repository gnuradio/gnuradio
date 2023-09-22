/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_PDU_RANDOM_PDU_IMPL_H
#define INCLUDED_PDU_RANDOM_PDU_IMPL_H

#include <gnuradio/pdu/random_pdu.h>

#include <random>

namespace gr {
namespace pdu {

class random_pdu_impl : public random_pdu
{
private:
    std::mt19937 d_rng;
    std::uniform_int_distribution<> d_urange;
    std::uniform_int_distribution<> d_brange;
    const unsigned char d_mask;
    const int d_length_modulo;

public:
    random_pdu_impl(int min_items,
                    int max_items,
                    unsigned char byte_mask,
                    int length_modulo);

    bool start() override;
    void output_random();
    void generate_pdu(pmt::pmt_t msg) { output_random(); }
    void generate_pdu() { output_random(); }
};

} /* namespace pdu */
} /* namespace gr */

#endif /* INCLUDED_PDU_RANDOM_PDU_IMPL_H */
