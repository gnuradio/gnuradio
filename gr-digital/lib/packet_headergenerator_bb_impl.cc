/* -*- c++ -*- */
/* Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "packet_headergenerator_bb_impl.h"
#include <gnuradio/io_signature.h>
#include <boost/format.hpp>

namespace gr {
namespace digital {

packet_headergenerator_bb::sptr
packet_headergenerator_bb::make(const packet_header_default::sptr& header_formatter,
                                const std::string& len_tag_key)
{
    return gnuradio::make_block_sptr<packet_headergenerator_bb_impl>(header_formatter,
                                                                     len_tag_key);
}


packet_headergenerator_bb::sptr
packet_headergenerator_bb::make(long header_len, const std::string& len_tag_key)
{
    const packet_header_default::sptr header_formatter(
        new packet_header_default(header_len, len_tag_key));
    return gnuradio::make_block_sptr<packet_headergenerator_bb_impl>(header_formatter,
                                                                     len_tag_key);
}


packet_headergenerator_bb_impl::packet_headergenerator_bb_impl(
    const gr::digital::packet_header_default::sptr& header_formatter,
    const std::string& len_tag_key)
    : tagged_stream_block("packet_headergenerator_bb_impl",
                          io_signature::make(1, 1, sizeof(char)),
                          io_signature::make(1, 1, sizeof(char)),
                          len_tag_key),
      d_formatter(header_formatter)
{
    set_output_multiple(d_formatter->header_len());
    // This is the worst case rate, because we don't know the true value, of course
    set_relative_rate((uint64_t)d_formatter->header_len(), 1);
    set_tag_propagation_policy(TPP_DONT);
}

void packet_headergenerator_bb_impl::set_header_formatter(
    packet_header_default::sptr header_formatter)
{
    gr::thread::scoped_lock guard(d_setlock);
    d_formatter = header_formatter;
}
packet_headergenerator_bb_impl::~packet_headergenerator_bb_impl() {}

int packet_headergenerator_bb_impl::work(int noutput_items,
                                         gr_vector_int& ninput_items,
                                         gr_vector_const_void_star& input_items,
                                         gr_vector_void_star& output_items)
{
    gr::thread::scoped_lock guard(d_setlock);
    unsigned char* out = (unsigned char*)output_items[0];

    std::vector<tag_t> tags;
    get_tags_in_range(tags, 0, nitems_read(0), nitems_read(0) + ninput_items[0]);
    if (!d_formatter->header_formatter(ninput_items[0], out, tags)) {
        GR_LOG_FATAL(d_logger,
                     boost::format("header_formatter() returned false (this shouldn't "
                                   "happen). Offending header started at %1%") %
                         nitems_read(0));
        throw std::runtime_error("header formatter returned false.");
    }

    return d_formatter->header_len();
}

} /* namespace digital */
} /* namespace gr */
