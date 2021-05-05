/* -*- c++ -*- */
/*
 * Copyright 2014 Analog Devices Inc.
 * Author: Paul Cercueil <paul.cercueil@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "fmcomms2_source_impl.h"
#include <gnuradio/blocks/float_to_complex.h>
#include <gnuradio/blocks/short_to_float.h>
#include <gnuradio/io_signature.h>
#include <ad9361.h>

#include <cstdio>
#include <string>
#include <thread>
#include <vector>

#define MIN_RATE 520333
#define DECINT_RATIO 8
#define OVERFLOW_CHECK_PERIOD_MS 1000

using namespace gr::blocks;

namespace gr {
namespace iio {

fmcomms2_source_f32c::fmcomms2_source_f32c(bool rx1_en,
                                           bool rx2_en,
                                           fmcomms2_source::sptr src_block)
    : hier_block2("fmcomms2_f32c",
                  io_signature::make(0, 0, 0),
                  io_signature::make((int)rx1_en + (int)rx2_en,
                                     (int)rx1_en + (int)rx2_en,
                                     sizeof(gr_complex))),
      fmcomms2_block(src_block)
{
    basic_block_sptr hier = shared_from_this();
    unsigned int num_streams = (int)rx1_en + (int)rx2_en;

    for (unsigned int i = 0; i < num_streams; i++) {
        short_to_float::sptr s2f1 = short_to_float::make(1, 2048.0);
        short_to_float::sptr s2f2 = short_to_float::make(1, 2048.0);
        float_to_complex::sptr f2c = float_to_complex::make(1);

        connect(src_block, i * 2, s2f1, 0);
        connect(src_block, i * 2 + 1, s2f2, 0);
        connect(s2f1, 0, f2c, 0);
        connect(s2f2, 0, f2c, 1);
        connect(f2c, 0, hier, i);
    }
}

fmcomms2_source::sptr fmcomms2_source::make(const std::string& uri,
                                            const std::vector<bool>& ch_en,
                                            unsigned long buffer_size)
{
    return gnuradio::make_block_sptr<fmcomms2_source_impl>(
        device_source_impl::get_context(uri), ch_en, buffer_size);
}

std::vector<std::string> fmcomms2_source_impl::get_channels_vector(bool ch1_en,
                                                                   bool ch2_en,
                                                                   bool ch3_en,
                                                                   bool ch4_en)
{
    std::vector<std::string> channels;
    if (ch1_en)
        channels.push_back("voltage0");
    if (ch2_en)
        channels.push_back("voltage1");
    if (ch3_en)
        channels.push_back("voltage2");
    if (ch4_en)
        channels.push_back("voltage3");
    return channels;
}

std::vector<std::string>
fmcomms2_source_impl::get_channels_vector(const std::vector<bool>& ch_en)
{
    std::vector<std::string> channels;
    int idx = 0;
    for (auto en : ch_en) {
        if (en) {
            channels.push_back("voltage" + std::to_string(idx));
        }
        idx++;
    }

    return channels;
}


fmcomms2_source_impl::fmcomms2_source_impl(struct iio_context* ctx,
                                           const std::vector<bool>& ch_en,
                                           unsigned long buffer_size)
    : gr::sync_block("fmcomms2_source",
                     gr::io_signature::make(0, 0, 0),
                     gr::io_signature::make(1, -1, sizeof(short))),
      device_source_impl(ctx,
                         true,
                         "cf-ad9361-lpc",
                         get_channels_vector(ch_en),
                         "ad9361-phy",
                         std::vector<std::string>(),
                         buffer_size,
                         0)
{
    overflow_thd = std::thread(&fmcomms2_source_impl::check_overflow, this);
}

fmcomms2_source_impl::~fmcomms2_source_impl() { overflow_thd.join(); }

void fmcomms2_source_impl::check_overflow(void)
{
    uint32_t status;
    int ret;

    // Wait for stream startup
#ifdef _WIN32
    while (thread_stopped) {
        Sleep(OVERFLOW_CHECK_PERIOD_MS);
    }
    Sleep(OVERFLOW_CHECK_PERIOD_MS);
#else
    while (thread_stopped) {
        usleep(OVERFLOW_CHECK_PERIOD_MS * 1000);
    }
    usleep(OVERFLOW_CHECK_PERIOD_MS * 1000);
#endif

    // Clear status registers
    iio_device_reg_write(dev, 0x80000088, 0x6);

    while (!thread_stopped) {
        ret = iio_device_reg_read(dev, 0x80000088, &status);
        if (ret) {
            throw std::runtime_error("Failed to read overflow status register");
        }
        if (status & 4) {
            printf("O");
            // Clear status registers
            iio_device_reg_write(dev, 0x80000088, 4);
        }
#ifdef _WIN32
        Sleep(OVERFLOW_CHECK_PERIOD_MS);
#else
        usleep(OVERFLOW_CHECK_PERIOD_MS * 1000);
#endif
    }
}

void fmcomms2_source_impl::update_dependent_params()
{
    std::vector<std::string> params;
    // Set rate configuration
    if (d_filter_source.compare("Off") == 0) {
        params.push_back("in_voltage_sampling_frequency=" + std::to_string(d_samplerate));
        params.push_back("in_voltage_rf_bandwidth=" + std::to_string(d_bandwidth));
    } else if (d_filter_source.compare("Auto") == 0) {
        int ret = ad9361_set_bb_rate(phy, d_samplerate);
        if (ret) {
            throw std::runtime_error("Unable to set BB rate");
            params.push_back("in_voltage_rf_bandwidth=" + std::to_string(d_bandwidth));
        }
    } else if (d_filter_source.compare("File") == 0) {
        std::string filt(d_filter_filename);
        if (!load_fir_filter(filt, phy))
            throw std::runtime_error("Unable to load filter file");
    } else if (d_filter_source.compare("Design") == 0) {
        int ret = ad9361_set_bb_rate_custom_filter_manual(
            phy, d_samplerate, d_fpass, d_fstop, d_bandwidth, d_bandwidth);
        if (ret) {
            throw std::runtime_error("Unable to set BB rate");
        }
    } else
        throw std::runtime_error("Unknown filter configuration");

    device_source_impl::set_params(params);
    // Filters can only be disabled after the sample rate has been set
    if (d_filter_source.compare("Off") == 0) {
        int ret = ad9361_set_trx_fir_enable(phy, false);
        if (ret) {
            throw std::runtime_error("Unable to disable filters");
        }
    }
}

void fmcomms2_source_impl::set_frequency(unsigned long long frequency)
{
    std::vector<std::string> params;
    params.push_back("out_altvoltage0_RX_LO_frequency=" + std::to_string(frequency));
    device_source_impl::set_params(params);
}

void fmcomms2_source_impl::set_samplerate(unsigned long samplerate)
{
    std::vector<std::string> params;
    if (samplerate < MIN_RATE) {
        int ret;
        samplerate = samplerate * DECINT_RATIO;
        ret = device_source_impl::handle_decimation_interpolation(
            samplerate, "voltage0", "sampling_frequency", dev, false);
        if (ret < 0)
            samplerate = samplerate / 8;
    } else // Disable decimation filter if on
    {
        device_source_impl::handle_decimation_interpolation(
            samplerate, "voltage0", "sampling_frequency", dev, true);
    }

    device_source_impl::set_params(params);
    d_samplerate = samplerate;
    update_dependent_params();
}

void fmcomms2_source_impl::set_gain_mode(size_t chan, const std::string& mode)
{
    bool is_fmcomms4 = !iio_device_find_channel(phy, "voltage1", false);
    if ((!is_fmcomms4 && chan > 0) || chan > 1) {
        throw std::runtime_error("Channel out of range for this device");
    }
    std::vector<std::string> params;

    params.push_back("in_voltage" + std::to_string(chan) +
                     "_gain_control_mode=" + d_gain_mode[chan]);

    device_source_impl::set_params(params);
    d_gain_mode[chan] = mode;
}


void fmcomms2_source_impl::set_gain(size_t chan, double gain_value)
{
    bool is_fmcomms4 = !iio_device_find_channel(phy, "voltage1", false);
    if ((!is_fmcomms4 && chan > 0) || chan > 1) {
        throw std::runtime_error("Channel out of range for this device");
    }
    std::vector<std::string> params;

    if (d_gain_mode[chan].compare("manual") == 0) {
        // params.push_back("in_voltage" + std::to_string(chan) +
        //                  "_gain_control_mode=" + d_gain_mode[chan]);
        params.push_back("in_voltage" + std::to_string(chan) +
                         "_hardwaregain=" + std::to_string(gain_value));
    }
    device_source_impl::set_params(params);
    d_gain_value[chan] = gain_value;
}

void fmcomms2_source_impl::set_quadrature(bool quadrature)
{
    std::vector<std::string> params;
    params.push_back("in_voltage_quadrature_tracking_en=" + std::to_string(quadrature));
    device_source_impl::set_params(params);
    d_quadrature = quadrature;
}
void fmcomms2_source_impl::set_rfdc(bool rfdc)
{
    std::vector<std::string> params;
    params.push_back("in_voltage_rf_dc_offset_tracking_en=" + std::to_string(rfdc));
    device_source_impl::set_params(params);
    d_rfdc = rfdc;
}
void fmcomms2_source_impl::set_bbdc(bool bbdc)
{
    std::vector<std::string> params;
    params.push_back("in_voltage_bb_dc_offset_tracking_en=" + std::to_string(bbdc));
    device_source_impl::set_params(params);
    d_bbdc = bbdc;
}
void fmcomms2_source_impl::set_filter_source(const std::string& filt_config)
{
    d_filter_source = filt_config;
    update_dependent_params();
}
void fmcomms2_source_impl::set_filter_filename(const std::string& filter_filename)
{
    d_filter_filename = filter_filename;
    update_dependent_params();
}
void fmcomms2_source_impl::set_fpass(float fpass)
{
    d_fpass = fpass;
    update_dependent_params();
}
void fmcomms2_source_impl::set_fstop(float fstop)
{
    d_fstop = fstop;
    update_dependent_params();
}

} /* namespace iio */
} /* namespace gr */
