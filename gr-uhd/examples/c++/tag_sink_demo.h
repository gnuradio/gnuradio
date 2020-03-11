/*
 * Copyright 2011 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <gnuradio/io_signature.h>
#include <gnuradio/sync_block.h>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <complex>
#include <iostream>

class tag_sink_demo : public gr::sync_block
{
public:
    tag_sink_demo(void)
        : sync_block("uhd tag sink demo",
                     gr::io_signature::make(1, 1, sizeof(std::complex<float>)),
                     gr::io_signature::make(0, 0, 0))
    {
        // NOP
    }

    int work(int ninput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items)
    {
        // grab all "rx time" tags in this work call
        const uint64_t samp0_count = this->nitems_read(0);
        std::vector<gr::tag_t> rx_time_tags;
        get_tags_in_range(rx_time_tags,
                          0,
                          samp0_count,
                          samp0_count + ninput_items,
                          pmt::string_to_symbol("rx_time"));

        // print all tags
        BOOST_FOREACH (const gr::tag_t& rx_time_tag, rx_time_tags) {
            const uint64_t offset = rx_time_tag.offset;
            const pmt::pmt_t& value = rx_time_tag.value;

            std::cout << boost::format(
                             "Full seconds %u, Frac seconds %f, abs sample offset %u") %
                             pmt::to_uint64(pmt::tuple_ref(value, 0)) %
                             pmt::to_double(pmt::tuple_ref(value, 1)) % offset
                      << std::endl;
        }

        return ninput_items;
    }
};
