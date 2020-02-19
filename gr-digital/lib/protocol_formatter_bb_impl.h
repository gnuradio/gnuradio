/* -*- c++ -*- */
/*
 * Copyright 2016 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_PROTOCOL_FORMATTER_BB_IMPL_H
#define INCLUDED_DIGITAL_PROTOCOL_FORMATTER_BB_IMPL_H

#include <gnuradio/digital/protocol_formatter_bb.h>

namespace gr {
namespace digital {

class protocol_formatter_bb_impl : public protocol_formatter_bb
{
private:
    header_format_base::sptr d_format;

public:
    protocol_formatter_bb_impl(const header_format_base::sptr& format,
                               const std::string& len_tag_key);
    ~protocol_formatter_bb_impl();

    void set_header_format(header_format_base::sptr& format);

    void remove_length_tags(const std::vector<std::vector<tag_t>>& tags){};
    int calculate_output_stream_length(const gr_vector_int& ninput_items)
    {
        return d_format->header_nbytes();
    };

    int work(int noutput_items,
             gr_vector_int& ninput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_PROTOCOL_FORMATTER_BB_IMPL_H */
