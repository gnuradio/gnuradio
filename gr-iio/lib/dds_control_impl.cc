/* -*- c++ -*- */
/*
 * Copyright 2019 Analog Devices Inc.
 * Author: Travis Collins <travis.collins@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "dds_control_impl.h"
#include <gnuradio/io_signature.h>

#include <string>
#include <vector>

namespace gr {
namespace iio {

dds_control::sptr dds_control::make(const std::string& uri,
                                    std::vector<int> enabled,
                                    std::vector<long> frequencies,
                                    std::vector<float> phases,
                                    std::vector<float> scales)
{
    return gnuradio::make_block_sptr<dds_control_impl>(
        uri, enabled, frequencies, phases, scales);
}

/*
 * The private constructor
 */
dds_control_impl::dds_control_impl(const std::string& uri,
                                   std::vector<int> enabled,
                                   std::vector<long> frequencies,
                                   std::vector<float> phases,
                                   std::vector<float> scales)
    : gr::block("dds_control",
                gr::io_signature::make(0, 0, 0),
                gr::io_signature::make(0, 0, 0)),
      d_enabled(enabled),
      d_frequencies(frequencies),
      d_phases(phases),
      d_scales(scales),
      d_uri(uri)
{
    int k, chans;
    unsigned int ku = 0, count = 0;
    iio_channel* chan;

    d_ctx = device_source_impl::get_context(uri);
    if (!d_ctx)
        throw std::runtime_error("Unable to create context");

    int d = iio_context_get_devices_count(d_ctx);
    iio_device* dev;
    const char* name;
    // Find dds device
    for (k = 0; k < d; k++) {
        dev = iio_context_get_device(d_ctx, k);
        name = iio_device_get_name(dev);
        std::string name_s(name);
        if (name_s.find("dds-core") != std::string::npos) {
            d_dev = dev;
            break;
        }
    }
    if (!d_dev) {
        iio_context_destroy(d_ctx);
        throw std::runtime_error("Device not found");
    }
    // Check that we have enough DDSs
    chans = iio_device_get_channels_count(d_dev);
    for (k = 0; k < chans; k++) {
        chan = iio_device_get_channel(d_dev, k);
        if (iio_channel_find_attr(chan, "raw") != NULL)
            count++;
    }
    for (ku = 1; ku <= enabled.size(); ku++)
        if ((count < ku * 4) && enabled[ku - 1] > 0)
            throw std::runtime_error(
                "Not enough DDSs available in hardware for configuration");

    set_dds_confg(frequencies, phases, scales);
}

void dds_control_impl::set_dds_confg(std::vector<long> frequencies,
                                     std::vector<float> phases,
                                     std::vector<float> scales)
{
    int ret, chans, k, dds_indx = 0, enable_indx = 0, enable = 0;
    unsigned int ku;
    iio_channel* chan;

    // Check vector sizes
    if ((frequencies.size() != phases.size()) || (phases.size() != scales.size()))
        throw std::runtime_error(
            "Frequencies, Phases, and Scales must be all the same size");

    for (ku = 0; ku < d_frequencies.size(); ku++) {
        d_frequencies[ku] = frequencies[ku];
        d_phases[ku] = phases[ku] * 1000;
        d_scales[ku] = scales[ku];
    }
    // DDS raw settings must be all on or all off
    for (ku = 0; ku < d_enabled.size(); ku++)
        enable += d_enabled[ku];

    // Enable/Disable DDSs
    chans = iio_device_get_channels_count(d_dev);
    for (k = 0; k < chans; k++) {

        chan = iio_device_get_channel(d_dev, k);

        if (iio_channel_find_attr(chan, "raw") != NULL) {

            ret = iio_channel_attr_write_longlong(
                chan, "frequency", d_frequencies[dds_indx]);
            if (ret < 0)
                d_logger->warn("Unable to set DDS frequency: {:d}",
                               d_frequencies[dds_indx]);

            ret = iio_channel_attr_write_longlong(chan, "phase", d_phases[dds_indx]);
            if (ret < 0)
                d_logger->warn("Unable to set DDS phase: {:g}", d_phases[dds_indx]);

            if (d_enabled[enable_indx])
                ret = iio_channel_attr_write_double(chan, "scale", d_scales[dds_indx]);
            else
                ret = iio_channel_attr_write_double(chan, "scale", 0);
            if (ret < 0)
                d_logger->warn("Unable to set DDS scale: {:g}", d_scales[dds_indx]);

            ret = iio_channel_attr_write_longlong(chan, "raw", enable);
            if (ret < 0)
                d_logger->warn("Unable to set DDS: {:d}", ret);

            dds_indx++;
            if ((dds_indx % 4) == 0)
                enable_indx++;
        }
    }
}

/*
 * Our virtual destructor.
 */
dds_control_impl::~dds_control_impl() {}

} /* namespace iio */
} /* namespace gr */
