/* -*- c++ -*- */
/* Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_PACKET_HEADERPARSER_B_IMPL_H
#define INCLUDED_DIGITAL_PACKET_HEADERPARSER_B_IMPL_H

#include <gnuradio/digital/packet_headerparser_b.h>

namespace gr {
namespace digital {

class packet_headerparser_b_impl : public packet_headerparser_b
{
private:
    packet_header_default::sptr d_header_formatter;

    const pmt::pmt_t d_port;

public:
    packet_headerparser_b_impl(
        const gr::digital::packet_header_default::sptr& header_formatter);
    ~packet_headerparser_b_impl();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_PACKET_HEADERPARSER_B_IMPL_H */
