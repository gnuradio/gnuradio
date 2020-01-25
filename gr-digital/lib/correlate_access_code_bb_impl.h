/* -*- c++ -*- */
/*
 * Copyright 2005,2006,2011,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_CORRELATE_ACCESS_CODE_BB_IMPL_H
#define INCLUDED_DIGITAL_CORRELATE_ACCESS_CODE_BB_IMPL_H

#include <gnuradio/digital/correlate_access_code_bb.h>

namespace gr {
namespace digital {

class correlate_access_code_bb_impl : public correlate_access_code_bb
{
private:
    unsigned long long d_access_code; // access code to locate start of packet
                                      //   access code is left justified in the word
    unsigned long long d_data_reg;    // used to look for access_code
    unsigned long long d_flag_reg;    // keep track of decisions
    unsigned long long d_flag_bit; // mask containing 1 bit which is location of new flag
    unsigned long long d_mask;     // masks access_code bits (top N bits are set where
                                   //   N is the number of bits in the access code)
    unsigned int d_threshold;      // how many bits may be wrong in sync vector

public:
    correlate_access_code_bb_impl(const std::string& access_code, int threshold);
    ~correlate_access_code_bb_impl();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);

    bool set_access_code(const std::string& access_code);
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_CORRELATE_ACCESS_CODE_BB_IMPL_H */
