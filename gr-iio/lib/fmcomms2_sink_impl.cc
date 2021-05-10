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
#include <gnuradio/blocks/complex_to_float.h>
#include <gnuradio/blocks/float_to_short.h>
#include <gnuradio/io_signature.h>
#include <ad9361.h>

#include <cstdio>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#define OVERFLOW_CHECK_PERIOD_MS 1000

using namespace gr::blocks;

namespace gr {
namespace iio {

fmcomms2_sink_f32c::fmcomms2_sink_f32c(bool tx1_en,
                                       bool tx2_en,
                                       fmcomms2_sink::sptr sink_block)
    : hier_block2("fmcomms2_sink_f32c",
                  io_signature::make((int)tx1_en + (int)tx2_en,
                                     (int)tx1_en + (int)tx2_en,
                                     sizeof(gr_complex)),
                  io_signature::make(0, 0, 0)),
      fmcomms2_block(sink_block)
{
    basic_block_sptr hier = shared_from_this();
    unsigned int num_streams = (int)tx1_en + (int)tx2_en;

    for (unsigned int i = 0; i < num_streams; i++) {
        float_to_short::sptr f2s1 = float_to_short::make(1, 32768.0);
        float_to_short::sptr f2s2 = float_to_short::make(1, 32768.0);
        complex_to_float::sptr c2f = complex_to_float::make();

        connect(hier, i, c2f, 0);
        connect(c2f, 0, f2s1, 0);
        connect(c2f, 1, f2s2, 0);
        connect(f2s1, 0, sink_block, i * 2);
        connect(f2s2, 0, sink_block, i * 2 + 1);
    }
}

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
    params.push_back("out_voltage_rf_bandwidth=" + boost::to_string(bandwidth));
    device_source_impl::set_params(this->phy, params);
    d_bandwidth = bandwidth;
}

void fmcomms2_sink_impl::set_rf_port_select(const std::string& rf_port_select)
{
    std::vector<std::string> params;
    params.push_back("out_voltage0_rf_port_select=" + boost::to_string(rf_port_select));
    device_source_impl::set_params(this->phy, params);
    d_rf_port_select = rf_port_select;
}
void fmcomms2_sink_impl::set_frequency(unsigned long long frequency)
{
    std::vector<std::string> params;
    params.push_back("out_altvoltage1_TX_LO_frequency=" + boost::to_string(frequency));
    device_source_impl::set_params(this->phy, params);
    d_frequency = frequency;
}
void fmcomms2_sink_impl::set_samplerate(unsigned long samplerate)
{
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
    bool is_fmcomms4 = !iio_device_find_channel(phy, "voltage1", false);
    std::vector<std::string> params;

    bool auto_filter = false;
    if (d_filter_filename != "")
        auto_filter = false;

    if (!auto_filter) {
        params.push_back("out_voltage_sampling_frequency=" +
                         boost::to_string(d_samplerate));
    }

    device_source_impl::set_params(this->phy, params);

    if (auto_filter) {
        int ret = ad9361_set_bb_rate(phy, d_samplerate);
        if (ret) {
            throw std::runtime_error("Unable to set BB rate");
        }
    } else if (d_filter_filename != "") {
        if (!device_source_impl::load_fir_filter(d_filter_filename, phy))
            throw std::runtime_error("Unable to load filter file");
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
