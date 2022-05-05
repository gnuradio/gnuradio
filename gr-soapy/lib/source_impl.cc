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

#include "source_impl.h"
#include <SoapySDR/Errors.hpp>
#include <iostream>

namespace gr {
namespace soapy {

source::sptr source::make(const std::string& device,
                          const std::string& type,
                          size_t nchan,
                          const std::string& dev_args,
                          const std::string& stream_args,
                          const std::vector<std::string>& tune_args,
                          const std::vector<std::string>& other_settings)
{
    return gnuradio::make_block_sptr<source_impl>(
        device, type, nchan, dev_args, stream_args, tune_args, other_settings);
}

source_impl::source_impl(const std::string& device,
                         const std::string& type,
                         size_t nchan,
                         const std::string& dev_args,
                         const std::string& stream_args,
                         const std::vector<std::string>& tune_args,
                         const std::vector<std::string>& other_settings)
    : gr::block("source", gr::io_signature::make(0, 0, 0), args_to_io_sig(type, nchan)),
      block_impl(SOAPY_SDR_RX,
                 device,
                 type,
                 nchan,
                 dev_args,
                 stream_args,
                 tune_args,
                 other_settings),
      _add_tag(false),
      _has_hardware_time(this->has_hardware_time(""))
{
}

bool source_impl::start()
{
    _add_tag = true;
    return block_impl::start();
}

void source_impl::set_frequency(size_t channel, const std::string& name, double frequency)
{
    block_impl::set_frequency(channel, name, frequency);
    _add_tag = true;
}
void source_impl::set_hardware_time(long long timeNs, const std::string& what)
{
    block_impl::set_hardware_time(timeNs, what);
    _add_tag = true;
}
void source_impl::set_sample_rate(size_t channel, double sample_rate)
{
    block_impl::set_sample_rate(channel, sample_rate);
    _add_tag = true;
}

int source_impl::general_work(int noutput_items,
                              [[maybe_unused]] gr_vector_int& ninput_items,
                              [[maybe_unused]] gr_vector_const_void_star& input_items,
                              gr_vector_void_star& output_items)
{
    long long int time_ns = -1;
    int flags = 0;
    const long timeout_us = 500000; // 0.5 sec
    int nout = 0;

    for (;;) {

        // No command handlers while reading
        int result;
        {
            std::lock_guard<std::mutex> l(d_device_mutex);
            result = d_device->readStream(
                d_stream, output_items.data(), noutput_items, flags, time_ns, timeout_us);
        }

        if (result >= 0) {
            if (_add_tag) {
                _add_tag = false;
                for (size_t i = 0; i < d_nchan; i++) {
                    if (_has_hardware_time) {
                        // Reproduce gr-uhd's 'rx_time' tuple from nano seconds.
                        const pmt::pmt_t time = pmt::make_tuple(
                            pmt::from_uint64(time_ns / 1000000000),
                            pmt::from_double((time_ns % 1000000000) / 1000000000.0L));
                        this->add_item_tag(i, nitems_written(0), TIME_KEY, time);
                    }
                    this->add_item_tag(i,
                                       nitems_written(0),
                                       RATE_KEY,
                                       pmt::from_double(this->get_sample_rate(i)));
                    this->add_item_tag(i,
                                       nitems_written(0),
                                       FREQ_KEY,
                                       pmt::from_double(this->get_frequency(i)));
                }
            }

            nout = result;
            break;
        }

        switch (result) {

        // Retry on overflow. Data has been lost.
        case SOAPY_SDR_OVERFLOW:
            _add_tag = true;
            std::cerr << "sO" << std::flush;
            continue;

        // Yield back to scheduler on timeout.
        case SOAPY_SDR_TIMEOUT:
            break;

        // Report and yield back to scheduler on other errors.
        default:
            d_logger->warn("Soapy source error: {:s}", SoapySDR::errToStr(result));
            break;
        }

        break;
    };

    return nout;
}

} /* namespace soapy */
} /* namespace gr */
