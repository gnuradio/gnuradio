/* -*- c++ -*- */
/* Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_PACKET_HEADERGENERATOR_BB_IMPL_H
#define INCLUDED_DIGITAL_PACKET_HEADERGENERATOR_BB_IMPL_H

#include <gnuradio/digital/packet_headergenerator_bb.h>

namespace gr {
namespace digital {

class packet_headergenerator_bb_impl : public packet_headergenerator_bb
{
private:
    gr::digital::packet_header_default::sptr d_formatter;

public:
    packet_headergenerator_bb_impl(const packet_header_default::sptr& header_formatter,
                                   const std::string& len_tag_key);
    ~packet_headergenerator_bb_impl();

    void set_header_formatter(packet_header_default::sptr header_formatter);

    void remove_length_tags(const std::vector<std::vector<tag_t>>& tags){};
    int calculate_output_stream_length(const gr_vector_int& ninput_items)
    {
        return d_formatter->header_len();
    };

    int work(int noutput_items,
             gr_vector_int& ninput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_PACKET_HEADERGENERATOR_BB_IMPL_H */
