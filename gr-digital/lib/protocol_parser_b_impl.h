/* -*- c++ -*- */
/*
 * Copyright 2015-2016 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_PROTOCOL_PARSER_B_IMPL_H
#define INCLUDED_DIGITAL_PROTOCOL_PARSER_B_IMPL_H

#include <gnuradio/digital/protocol_parser_b.h>

namespace gr {
namespace digital {

class protocol_parser_b_impl : public protocol_parser_b
{
private:
    header_format_base::sptr d_format;
    pmt::pmt_t d_out_port;

public:
    protocol_parser_b_impl(const header_format_base::sptr& format);
    ~protocol_parser_b_impl();

    void set_threshold(unsigned int thresh);
    unsigned int threshold() const;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_PROTOCOL_PARSER_B_IMPL_H */
