/* -*- c++ -*- */
/*
 * Copyright 2005,2006,2011,2012,2017 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_CORRELATE_ACCESS_CODE_TAG_FF_IMPL_H
#define INCLUDED_DIGITAL_CORRELATE_ACCESS_CODE_TAG_FF_IMPL_H

#include <gnuradio/digital/correlate_access_code_tag_ff.h>

namespace gr {
namespace digital {

class correlate_access_code_tag_ff_impl : public correlate_access_code_tag_ff
{
private:
    unsigned long long d_access_code; // access code to locate start of packet
                                      //   access code is left justified in the word
    unsigned long long d_data_reg;    // used to look for access_code
    unsigned long long d_mask;        // masks access_code bits (top N bits are set where
                                      //   N is the number of bits in the access code)
    unsigned int d_threshold;         // how many bits may be wrong in sync vector
    unsigned int d_len;               // the length of the access code

    pmt::pmt_t d_key, d_me; // d_key is the tag name, d_me is the block name + unique ID

    gr::thread::mutex d_mutex_access_code;

public:
    correlate_access_code_tag_ff_impl(const std::string& access_code,
                                      int threshold,
                                      const std::string& tag_name);
    ~correlate_access_code_tag_ff_impl();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);

    bool set_access_code(const std::string& access_code);
    void set_threshold(int threshold) { d_threshold = threshold; };
    void set_tagname(const std::string& tag_name)
    {
        d_key = pmt::string_to_symbol(tag_name);
    };
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_CORRELATE_ACCESS_CODE_TAG_FF_IMPL_H */
