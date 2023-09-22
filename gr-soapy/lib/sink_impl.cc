/* -*- c++ -*- */
/*
 * Copyright 2021 Jeff Long
 * Copyright 2018-2021 Libre Space Foundation <http://libre.space>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "sink_impl.h"
#include <SoapySDR/Errors.hpp>
#include <iostream>

namespace gr {
namespace soapy {

sink::sptr sink::make(const std::string& device,
                      const std::string& type,
                      size_t nchan,
                      const std::string& dev_args,
                      const std::string& stream_args,
                      const std::vector<std::string>& tune_args,
                      const std::vector<std::string>& other_settings)
{
    return gnuradio::make_block_sptr<sink_impl>(
        device, type, nchan, dev_args, stream_args, tune_args, other_settings);
}

sink_impl::sink_impl(const std::string& device,
                     const std::string& type,
                     size_t nchan,
                     const std::string& dev_args,
                     const std::string& stream_args,
                     const std::vector<std::string>& tune_args,
                     const std::vector<std::string>& other_settings)
    : gr::block("sink", args_to_io_sig(type, nchan), gr::io_signature::make(0, 0, 0)),
      block_impl(SOAPY_SDR_TX,
                 device,
                 type,
                 nchan,
                 dev_args,
                 stream_args,
                 tune_args,
                 other_settings)
{
}

void sink_impl::set_length_tag_name(const std::string& length_tag_name)
{
    d_length_tag_key = pmt::mp(length_tag_name);
}

int sink_impl::general_work([[maybe_unused]] int noutput_items,
                            gr_vector_int& ninput_items,
                            gr_vector_const_void_star& input_items,
                            [[maybe_unused]] gr_vector_void_star& output_items)
{
    int nin = ninput_items[0];
    long long int time_ns = 0;
    int nconsumed = 0;

    int nwrite = nin;
    int flags = 0;

    // TODO: optimization: add loop to handle portions of more than one burst
    // per call.

    // If using tagged bursts
    if (!pmt::is_null(d_length_tag_key)) {

        // If there are any length tags in the window, look at the first one only.
        std::vector<tag_t> length_tags;
        get_tags_in_window(length_tags, 0, 0, nin, d_length_tag_key);
        auto tag = length_tags.begin();
        if (tag != length_tags.end()) {

            long offset = tag->offset - nitems_read(0);

            // Current burst is done, new tag follows immediately.
            if (d_burst_remaining == 0 && offset == 0) {
                d_burst_remaining = pmt::to_long(tag->value);
            }

            // A length tag appears before the end of the current burst, alert
            // user of tag preemption. This may mean that samples have been
            // dropped somewhere. Adjust d_burst_remaining to end before tag.
            // The remaining samples in the current burst will still be written.
            else if (offset < d_burst_remaining) {
                std::cerr << "tP" << std::flush;
                d_burst_remaining = offset;
            }
        }

        // No samples remaining in burst means there was a tag gap. Alert user
        // and consume samples.
        if (d_burst_remaining == 0) {
            std::cerr << "tG" << std::flush;
            nconsumed += nin;
        }

        nwrite = std::min<long>(d_burst_remaining, nin);

        // If a burst is active, check if we finish it on this call
        if (d_burst_remaining == nwrite) {
            flags |= SOAPY_SDR_END_BURST;
        }
    }

    int result = 0;
    if (nwrite != 0) {
        // No command handlers while writing
        std::lock_guard<std::mutex> l(d_device_mutex);
        result =
            d_device->writeStream(d_stream, input_items.data(), nwrite, flags, time_ns);
    }

    if (result >= 0) {
        nconsumed += result;
        if (d_burst_remaining > 0) {
            d_burst_remaining -= result;
        }
    } else if (result == SOAPY_SDR_UNDERFLOW) {
        std::cerr << "sU" << std::flush;
    } else {
        d_logger->warn("Soapy sink error: {:s}", SoapySDR::errToStr(result));
    }

    consume_each(nconsumed);

    return 0;
}

} /* namespace soapy */
} /* namespace gr */
