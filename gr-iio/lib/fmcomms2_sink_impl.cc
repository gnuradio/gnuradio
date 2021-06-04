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

#include "device_source_impl.h"
#include "fmcomms2_sink_impl.h"
#include <gnuradio/io_signature.h>
#include <ad9361.h>

#include <cstdio>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#define MIN_RATE 520333
#define DECINT_RATIO 8
#define OVERFLOW_CHECK_PERIOD_MS 1000

namespace gr {
namespace iio {

fmcomms2_sink::sptr fmcomms2_sink::make(const std::string& uri,
                                        const std::vector<bool>& ch_en,
                                        unsigned long buffer_size,
                                        bool cyclic)
{
    return gnuradio::get_initial_sptr(new fmcomms2_sink_impl(
        device_source_impl::get_context(uri), ch_en, buffer_size, cyclic));
}

std::vector<std::string> fmcomms2_sink_impl::get_channels_vector(bool ch1_en,
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
fmcomms2_sink_impl::get_channels_vector(const std::vector<bool>& ch_en)
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


fmcomms2_sink_impl::fmcomms2_sink_impl(struct iio_context* ctx,
                                       const std::vector<bool>& ch_en,
                                       unsigned long buffer_size,
                                       bool cyclic)
    : gr::sync_block("fmcomms2_sink",
                     gr::io_signature::make(1, -1, sizeof(short)),
                     gr::io_signature::make(0, 0, 0)),
      device_sink_impl(ctx,
                       true,
                       "cf-ad9361-dds-core-lpc",
                       get_channels_vector(ch_en),
                       "ad9361-phy",
                       std::vector<std::string>(),
                       buffer_size,
                       0,
                       cyclic),
      cyclic(cyclic)
{
    stop_thread = false;
    underflow_thd = std::thread(&fmcomms2_sink_impl::check_underflow, this);
}

fmcomms2_sink_impl::~fmcomms2_sink_impl()
{
    std::unique_lock<std::mutex> lock(uf_mutex);
    stop_thread = true;
    lock.unlock();
    underflow_thd.join();
}

void fmcomms2_sink_impl::set_len_tag_key(const std::string& str)
{
    device_sink_impl::set_len_tag_key(str);
}

void fmcomms2_sink_impl::check_underflow(void)
{
    uint32_t status;
    int ret;
    std::unique_lock<std::mutex> lock(uf_mutex, std::defer_lock);

    // Clear status registers
    iio_device_reg_write(dev, 0x80000088, 0x6);

    for (;;) {
        ret = iio_device_reg_read(dev, 0x80000088, &status);
        if (ret) {
            throw std::runtime_error("Failed to read underflow status register");
        }
        if (status & 1) {
            printf("U");
            // Clear status registers
            iio_device_reg_write(dev, 0x80000088, 1);
        }
#ifdef _WIN32
        Sleep(OVERFLOW_CHECK_PERIOD_MS);
#else
        usleep(OVERFLOW_CHECK_PERIOD_MS * 1000);
#endif
        lock.lock();
        if (stop_thread)
            break;
        lock.unlock();
    }
}

void fmcomms2_sink_impl::set_bandwidth(unsigned long bandwidth)
{
    std::vector<std::string> params;
    params.push_back("out_voltage_rf_bandwidth=" + std::to_string(bandwidth));
    device_source_impl::set_params(this->phy, params);
    d_bandwidth = bandwidth;
}

void fmcomms2_sink_impl::set_rf_port_select(const std::string& rf_port_select)
{
    std::vector<std::string> params;
    params.push_back("out_voltage0_rf_port_select=" + rf_port_select);
    device_source_impl::set_params(this->phy, params);
    d_rf_port_select = rf_port_select;
}
void fmcomms2_sink_impl::set_frequency(unsigned long long frequency)
{
    std::vector<std::string> params;
    params.push_back("out_altvoltage1_TX_LO_frequency=" + std::to_string(frequency));
    device_source_impl::set_params(this->phy, params);
    d_frequency = frequency;
}
void fmcomms2_sink_impl::set_samplerate(unsigned long samplerate)
{
    std::vector<std::string> params;
    if (samplerate < MIN_RATE) {
        int ret;
        samplerate = samplerate * DECINT_RATIO;
        ret = device_source_impl::handle_decimation_interpolation(
            samplerate, "voltage0", "sampling_frequency", dev, false, true);
        if (ret < 0)
            samplerate = samplerate / 8;
    } else // Disable decimation filter if on
    {
        device_source_impl::handle_decimation_interpolation(
            samplerate, "voltage0", "sampling_frequency", dev, true, true);
    }

    device_source_impl::set_params(this->phy, params);
    d_samplerate = samplerate;
    update_dependent_params();
}
void fmcomms2_sink_impl::set_attenuation(size_t chan, double attenuation)
{
    bool is_fmcomms4 = !iio_device_find_channel(phy, "voltage1", false);
    if ((!is_fmcomms4 && chan > 0) || chan > 1) {
        throw std::runtime_error("Channel out of range for this device");
    }
    std::vector<std::string> params;

    params.push_back("out_voltage" + std::to_string(chan) +
                     "_hardwaregain=" + std::to_string(-attenuation));


    device_source_impl::set_params(this->phy, params);

    d_attenuation[chan] = attenuation;
}

void fmcomms2_sink_impl::update_dependent_params()
{
    std::vector<std::string> params;
    // Set rate configuration
    if (d_filter_source.compare("Off") == 0) {
        params.push_back("out_voltage_sampling_frequency=" +
                         std::to_string(d_samplerate));
        params.push_back("out_voltage_rf_bandwidth=" + std::to_string(d_bandwidth));
    } else if (d_filter_source.compare("Auto") == 0) {
        int ret = ad9361_set_bb_rate(phy, d_samplerate);
        if (ret) {
            throw std::runtime_error("Unable to set BB rate");
            params.push_back("out_voltage_rf_bandwidth=" + std::to_string(d_bandwidth));
        }
    } else if (d_filter_source.compare("File") == 0) {
        std::string filt(d_filter_filename);
        if (!device_source_impl::load_fir_filter(filt, phy))
            throw std::runtime_error("Unable to load filter file");
    } else if (d_filter_source.compare("Design") == 0) {
        int ret = ad9361_set_bb_rate_custom_filter_manual(
            phy, d_samplerate, d_fpass, d_fstop, d_bandwidth, d_bandwidth);
        if (ret) {
            throw std::runtime_error("Unable to set BB rate");
        }
    } else
        throw std::runtime_error("Unknown filter configuration");

    device_source_impl::set_params(this->phy, params);
    // Filters can only be disabled after the sample rate has been set
    if (d_filter_source.compare("Off") == 0) {
        int ret = ad9361_set_trx_fir_enable(phy, false);
        if (ret) {
            throw std::runtime_error("Unable to disable filters");
        }
    }
}

void fmcomms2_sink_impl::set_filter_params(const std::string& filter_source,
                                           const std::string& filter_filename,
                                           float fpass,
                                           float fstop)
{
    d_filter_source = filter_source;
    d_filter_filename = filter_filename;
    d_fpass = fpass;
    d_fstop = fstop;
    update_dependent_params();
}

int fmcomms2_sink_impl::work(int noutput_items,
                             gr_vector_const_void_star& input_items,
                             gr_vector_void_star& output_items)
{
    int ret = device_sink_impl::work(noutput_items, input_items, output_items);
    if (ret < 0 || !cyclic)
        return ret;
    else
        return WORK_DONE;
}
} /* namespace iio */
} /* namespace gr */
