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

#include <cstdio>

#include "fmcomms2_source_impl.h"
#include <gnuradio/io_signature.h>

#include <gnuradio/blocks/float_to_complex.h>
#include <gnuradio/blocks/short_to_float.h>

#include <ad9361.h>

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
                                            unsigned long long frequency,
                                            unsigned long samplerate,
                                            unsigned long bandwidth,
                                            bool ch1_en,
                                            bool ch2_en,
                                            bool ch3_en,
                                            bool ch4_en,
                                            unsigned long buffer_size,
                                            bool quadrature,
                                            bool rfdc,
                                            bool bbdc,
                                            const char* gain1,
                                            double gain1_value,
                                            const char* gain2,
                                            double gain2_value,
                                            const char* port_select,
                                            const char* filter_source,
                                            const char* filter_filename,
                                            float Fpass,
                                            float Fstop)
{
    return gnuradio::get_initial_sptr(
        new fmcomms2_source_impl(device_source_impl::get_context(uri),
                                 true,
                                 frequency,
                                 samplerate,
                                 bandwidth,
                                 ch1_en,
                                 ch2_en,
                                 ch3_en,
                                 ch4_en,
                                 buffer_size,
                                 quadrature,
                                 rfdc,
                                 bbdc,
                                 gain1,
                                 gain1_value,
                                 gain2,
                                 gain2_value,
                                 port_select,
                                 filter_source,
                                 filter_filename,
                                 Fpass,
                                 Fstop));
}

fmcomms2_source::sptr fmcomms2_source::make_from(struct iio_context* ctx,
                                                 unsigned long long frequency,
                                                 unsigned long samplerate,
                                                 unsigned long bandwidth,
                                                 bool ch1_en,
                                                 bool ch2_en,
                                                 bool ch3_en,
                                                 bool ch4_en,
                                                 unsigned long buffer_size,
                                                 bool quadrature,
                                                 bool rfdc,
                                                 bool bbdc,
                                                 const char* gain1,
                                                 double gain1_value,
                                                 const char* gain2,
                                                 double gain2_value,
                                                 const char* port_select,
                                                 const char* filter_source,
                                                 const char* filter_filename,
                                                 float Fpass,
                                                 float Fstop)
{
    return gnuradio::get_initial_sptr(new fmcomms2_source_impl(ctx,
                                                               false,
                                                               frequency,
                                                               samplerate,
                                                               bandwidth,
                                                               ch1_en,
                                                               ch2_en,
                                                               ch3_en,
                                                               ch4_en,
                                                               buffer_size,
                                                               quadrature,
                                                               rfdc,
                                                               bbdc,
                                                               gain1,
                                                               gain1_value,
                                                               gain2,
                                                               gain2_value,
                                                               port_select,
                                                               filter_source,
                                                               filter_filename,
                                                               Fpass,
                                                               Fstop));
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

fmcomms2_source_impl::fmcomms2_source_impl(struct iio_context* ctx,
                                           bool destroy_ctx,
                                           unsigned long long frequency,
                                           unsigned long samplerate,
                                           unsigned long bandwidth,
                                           bool ch1_en,
                                           bool ch2_en,
                                           bool ch3_en,
                                           bool ch4_en,
                                           unsigned long buffer_size,
                                           bool quadrature,
                                           bool rfdc,
                                           bool bbdc,
                                           const char* gain1,
                                           double gain1_value,
                                           const char* gain2,
                                           double gain2_value,
                                           const char* port_select,
                                           const char* filter_source,
                                           const char* filter_filename,
                                           float Fpass,
                                           float Fstop)
    : gr::sync_block("fmcomms2_source",
                     gr::io_signature::make(0, 0, 0),
                     gr::io_signature::make(1, -1, sizeof(short))),
      device_source_impl(ctx,
                         destroy_ctx,
                         "cf-ad9361-lpc",
                         get_channels_vector(ch1_en, ch2_en, ch3_en, ch4_en),
                         "ad9361-phy",
                         std::vector<std::string>(),
                         buffer_size,
                         0)
{
    set_params(frequency,
               samplerate,
               bandwidth,
               quadrature,
               rfdc,
               bbdc,
               gain1,
               gain1_value,
               gain2,
               gain2_value,
               port_select,
               filter_source,
               filter_filename,
               Fpass,
               Fstop);

    overflow_thd = boost::thread(&fmcomms2_source_impl::check_overflow, this);
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

void fmcomms2_source_impl::set_params(unsigned long long frequency,
                                      unsigned long samplerate,
                                      unsigned long bandwidth,
                                      bool quadrature,
                                      bool rfdc,
                                      bool bbdc,
                                      const char* gain1,
                                      double gain1_value,
                                      const char* gain2,
                                      double gain2_value,
                                      const char* port_select,
                                      const char* filter_source,
                                      const char* filter_filename,
                                      float Fpass,
                                      float Fstop)
{
    bool is_fmcomms4 = !iio_device_find_channel(phy, "voltage1", false);
    std::vector<std::string> params;
    int ret;

    if (samplerate < MIN_RATE) {
        int ret;
        samplerate = samplerate * DECINT_RATIO;
        ret = device_source_impl::handle_decimation_interpolation(
            samplerate, "voltage0", "sampling_frequency", dev, false);
        if (ret < 0)
            samplerate = samplerate / 8;
    } else // Disable decimation filter if on
        device_source_impl::handle_decimation_interpolation(
            samplerate, "voltage0", "sampling_frequency", dev, true);

    params.push_back("out_altvoltage0_RX_LO_frequency=" + boost::to_string(frequency));
    params.push_back("in_voltage_quadrature_tracking_en=" + boost::to_string(quadrature));
    params.push_back("in_voltage_rf_dc_offset_tracking_en=" + boost::to_string(rfdc));
    params.push_back("in_voltage_bb_dc_offset_tracking_en=" + boost::to_string(bbdc));
    std::string gain1_str = boost::to_string(gain1);
    params.push_back("in_voltage0_gain_control_mode=" + gain1_str);
    if (gain1_str.compare("manual") == 0) {
        params.push_back("in_voltage0_hardwaregain=" + boost::to_string(gain1_value));
    }

    // Set rate configuration
    std::string filt_config(filter_source);
    if (filt_config.compare("Off") == 0) {
        params.push_back("in_voltage_sampling_frequency=" + boost::to_string(samplerate));
        params.push_back("in_voltage_rf_bandwidth=" + boost::to_string(bandwidth));
    } else if (filt_config.compare("Auto") == 0) {
        int ret = ad9361_set_bb_rate(phy, samplerate);
        if (ret) {
            throw std::runtime_error("Unable to set BB rate");
            params.push_back("in_voltage_rf_bandwidth=" + boost::to_string(bandwidth));
        }
    } else if (filt_config.compare("File") == 0) {
        std::string filt(filter_filename);
        if (!load_fir_filter(filt, phy))
            throw std::runtime_error("Unable to load filter file");
    } else if (filt_config.compare("Design") == 0) {
        ret = ad9361_set_bb_rate_custom_filter_manual(
            phy, samplerate, Fpass, Fstop, bandwidth, bandwidth);
        if (ret) {
            throw std::runtime_error("Unable to set BB rate");
        }
    } else
        throw std::runtime_error("Unknown filter configuration");

    if (!is_fmcomms4) {
        std::string gain2_str = boost::to_string(gain2);
        params.push_back("in_voltage1_gain_control_mode=" + gain2_str);
        if (gain2_str.compare("manual") == 0) {
            params.push_back("in_voltage1_hardwaregain=" + boost::to_string(gain2_value));
        }
    }
    params.push_back("in_voltage0_rf_port_select=" + boost::to_string(port_select));

    device_source_impl::set_params(params);
    // Filters can only be disabled after the sample rate has been set
    if (filt_config.compare("Off") == 0) {
        ret = ad9361_set_trx_fir_enable(phy, false);
        if (ret) {
            throw std::runtime_error("Unable to disable fitlers");
        }
    }
}

} /* namespace iio */
} /* namespace gr */
