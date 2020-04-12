/*
 * Copyright 2011 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <gnuradio/io_signature.h>
#include <gnuradio/logger.h>
#include <gnuradio/sync_block.h>
#include <boost/format.hpp>
#include <complex>
#include <iostream>

class tag_sink_demo : public gr::sync_block
{
private:
    gr::logger_ptr d_logger, d_debug_logger;

public:
    tag_sink_demo(void)
        : sync_block("uhd tag sink demo",
                     gr::io_signature::make(1, 1, sizeof(std::complex<float>)),
                     gr::io_signature::make(0, 0, 0))
    {
        gr::configure_default_loggers(d_logger, d_debug_logger, "tag_sink_demo");
    }

    int work(int ninput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override
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
        auto format_string =
            boost::format("Full seconds %u, Frac seconds %f, abs sample offset %u");
        for (const auto& rx_time_tag : rx_time_tags) {
            const uint64_t offset = rx_time_tag.offset;
            const pmt::pmt_t& value = rx_time_tag.value;

            GR_LOG_INFO(d_logger,
                        format_string % pmt::to_uint64(pmt::tuple_ref(value, 0)) %
                            pmt::to_double(pmt::tuple_ref(value, 1)) % offset);
        }

        return ninput_items;
    }
};
