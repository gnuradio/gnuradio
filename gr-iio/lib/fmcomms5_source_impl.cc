/* -*- c++ -*- */
/*
 * Copyright 2015 Analog Devices Inc.
 * Author: Paul Cercueil <paul.cercueil@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "fmcomms5_source_impl.h"
#include <gnuradio/blocks/float_to_complex.h>
#include <gnuradio/blocks/short_to_float.h>
#include <gnuradio/io_signature.h>
#include <ad9361.h>

#include <string>
#include <vector>

using namespace gr::blocks;

namespace gr {
namespace iio {

fmcomms5_source_f32c::fmcomms5_source_f32c(
    bool rx1_en, bool rx2_en, bool rx3_en, bool rx4_en, fmcomms5_source::sptr src_block)
    : hier_block2(
          "fmcomms5_f32c",
          io_signature::make(0, 0, 0),
          io_signature::make((int)rx1_en + (int)rx2_en + (int)rx3_en + (int)rx4_en,
                             (int)rx1_en + (int)rx2_en + (int)rx3_en + (int)rx4_en,
                             sizeof(gr_complex))),
      fmcomms5_block(src_block)
{
    basic_block_sptr hier = shared_from_this();
    unsigned int num_streams = (int)rx1_en + (int)rx2_en + (int)rx3_en + (int)rx4_en;

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

fmcomms5_source::sptr fmcomms5_source::make(const std::string& uri,
                                            unsigned long long frequency1,
                                            unsigned long long frequency2,
                                            unsigned long samplerate,
                                            unsigned long bandwidth,
                                            bool ch1_en,
                                            bool ch2_en,
                                            bool ch3_en,
                                            bool ch4_en,
                                            bool ch5_en,
                                            bool ch6_en,
                                            bool ch7_en,
                                            bool ch8_en,
                                            unsigned long buffer_size,
                                            bool quadrature,
                                            bool rfdc,
                                            bool bbdc,
                                            const char* gain1,
                                            double gain1_value,
                                            const char* gain2,
                                            double gain2_value,
                                            const char* gain3,
                                            double gain3_value,
                                            const char* gain4,
                                            double gain4_value,
                                            const char* port_select,
                                            const char* filter_source,
                                            const char* filter_filename,
                                            float Fpass,
                                            float Fstop)
{
    return gnuradio::make_block_sptr<fmcomms5_source_impl>(
        device_source_impl::get_context(uri),
        true,
        frequency1,
        frequency2,
        samplerate,
        bandwidth,
        ch1_en,
        ch2_en,
        ch3_en,
        ch4_en,
        ch5_en,
        ch6_en,
        ch7_en,
        ch8_en,
        buffer_size,
        quadrature,
        rfdc,
        bbdc,
        gain1,
        gain1_value,
        gain2,
        gain2_value,
        gain3,
        gain3_value,
        gain4,
        gain4_value,
        port_select,
        filter_source,
        filter_filename,
        Fpass,
        Fstop);
}

fmcomms5_source::sptr fmcomms5_source::make_from(iio_context* ctx,
                                                 unsigned long long frequency1,
                                                 unsigned long long frequency2,
                                                 unsigned long samplerate,
                                                 unsigned long bandwidth,
                                                 bool ch1_en,
                                                 bool ch2_en,
                                                 bool ch3_en,
                                                 bool ch4_en,
                                                 bool ch5_en,
                                                 bool ch6_en,
                                                 bool ch7_en,
                                                 bool ch8_en,
                                                 unsigned long buffer_size,
                                                 bool quadrature,
                                                 bool rfdc,
                                                 bool bbdc,
                                                 const char* gain1,
                                                 double gain1_value,
                                                 const char* gain2,
                                                 double gain2_value,
                                                 const char* gain3,
                                                 double gain3_value,
                                                 const char* gain4,
                                                 double gain4_value,
                                                 const char* port_select,
                                                 const char* filter_source,
                                                 const char* filter_filename,
                                                 float Fpass,
                                                 float Fstop)
{
    return gnuradio::make_block_sptr<fmcomms5_source_impl>(ctx,
                                                           false,
                                                           frequency1,
                                                           frequency2,
                                                           samplerate,
                                                           bandwidth,
                                                           ch1_en,
                                                           ch2_en,
                                                           ch3_en,
                                                           ch4_en,
                                                           ch5_en,
                                                           ch6_en,
                                                           ch7_en,
                                                           ch8_en,
                                                           buffer_size,
                                                           quadrature,
                                                           rfdc,
                                                           bbdc,
                                                           gain1,
                                                           gain1_value,
                                                           gain2,
                                                           gain2_value,
                                                           gain3,
                                                           gain3_value,
                                                           gain4,
                                                           gain4_value,
                                                           port_select,
                                                           filter_source,
                                                           filter_filename,
                                                           Fpass,
                                                           Fstop);
}

std::vector<std::string> fmcomms5_source_impl::get_channels_vector(bool ch1_en,
                                                                   bool ch2_en,
                                                                   bool ch3_en,
                                                                   bool ch4_en,
                                                                   bool ch5_en,
                                                                   bool ch6_en,
                                                                   bool ch7_en,
                                                                   bool ch8_en)
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
    if (ch5_en)
        channels.push_back("voltage4");
    if (ch6_en)
        channels.push_back("voltage5");
    if (ch7_en)
        channels.push_back("voltage6");
    if (ch8_en)
        channels.push_back("voltage7");
    return channels;
}

fmcomms5_source_impl::fmcomms5_source_impl(iio_context* ctx,
                                           bool destroy_ctx,
                                           unsigned long long frequency1,
                                           unsigned long long frequency2,
                                           unsigned long samplerate,
                                           unsigned long bandwidth,
                                           bool ch1_en,
                                           bool ch2_en,
                                           bool ch3_en,
                                           bool ch4_en,
                                           bool ch5_en,
                                           bool ch6_en,
                                           bool ch7_en,
                                           bool ch8_en,
                                           unsigned long buffer_size,
                                           bool quadrature,
                                           bool rfdc,
                                           bool bbdc,
                                           const char* gain1,
                                           double gain1_value,
                                           const char* gain2,
                                           double gain2_value,
                                           const char* gain3,
                                           double gain3_value,
                                           const char* gain4,
                                           double gain4_value,
                                           const char* port_select,
                                           const char* filter_source,
                                           const char* filter_filename,
                                           float Fpass,
                                           float Fstop)
    : gr::sync_block("fmcomms5_source",
                     gr::io_signature::make(0, 0, 0),
                     gr::io_signature::make(1, -1, sizeof(short))),
      device_source_impl(
          ctx,
          destroy_ctx,
          "cf-ad9361-A",
          get_channels_vector(
              ch1_en, ch2_en, ch3_en, ch4_en, ch5_en, ch6_en, ch7_en, ch8_en),
          "ad9361-phy",
          std::vector<std::string>(),
          buffer_size,
          0),
      samplerate(0)
{
    phy2 = iio_context_find_device(ctx, "ad9361-phy-B");
    if (!phy2)
        throw std::runtime_error("Device not found");

    set_params(frequency1,
               frequency2,
               samplerate,
               bandwidth,
               quadrature,
               rfdc,
               bbdc,
               gain1,
               gain1_value,
               gain2,
               gain2_value,
               gain3,
               gain3_value,
               gain4,
               gain4_value,
               port_select,
               filter_source,
               filter_filename,
               Fpass,
               Fstop);
}

void fmcomms5_source_impl::set_params(iio_device* phy_device,
                                      unsigned long long frequency,
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
    iio_param_vec_t params;
    std::string gain1_str(gain1);
    std::string gain2_str(gain2);
    int ret;

    params.emplace_back("out_altvoltage0_RX_LO_frequency", frequency);
    params.emplace_back("in_voltage_quadrature_tracking_en", quadrature);
    params.emplace_back("in_voltage_rf_dc_offset_tracking_en", rfdc);
    params.emplace_back("in_voltage_bb_dc_offset_tracking_en", bbdc);
    params.emplace_back("in_voltage0_gain_control_mode", gain1);
    if (gain1_str.compare("manual") == 0)
        params.emplace_back("in_voltage0_hardwaregain", gain1_value);
    params.emplace_back("in_voltage1_gain_control_mode", gain2);
    if (gain2_str.compare("manual") == 0)
        params.emplace_back("in_voltage1_hardwaregain", gain2_value);
    params.emplace_back("in_voltage0_rf_port_select", port_select);

    // Set rate configuration
    std::string filt_config(filter_source);
    if (filt_config.compare("Off") == 0) {
        params.emplace_back("in_voltage_sampling_frequency", samplerate);
        params.emplace_back("in_voltage_rf_bandwidth", bandwidth);
    } else if (filt_config.compare("Auto") == 0) {
        params.emplace_back("in_voltage_rf_bandwidth", bandwidth);
        ret = ad9361_set_bb_rate(phy_device, samplerate);
        if (ret)
            throw std::runtime_error("Unable to set BB rate");
    } else if (filt_config.compare("File") == 0) {
        std::string filt(filter_filename);
        if (!filt.empty() && !load_fir_filter(filt, phy_device))
            throw std::runtime_error("Unable to load filter file");
    } else if (filt_config.compare("Design") == 0) {
        ret = ad9361_set_bb_rate_custom_filter_manual(
            phy_device, samplerate, Fpass, Fstop, bandwidth, bandwidth);
        if (ret)
            throw std::runtime_error("Unable to set BB rate");
    } else
        throw std::runtime_error("Unknown filter configuration");

    device_source_impl::set_params(phy_device, params);

    // Filters can only be disabled after the sample rate has been set
    if (filt_config.compare("Off") == 0) {
        ret = ad9361_set_trx_fir_enable(phy_device, false);
        if (ret) {
            throw std::runtime_error("Unable to disable filters");
        }
    }
}

void fmcomms5_source_impl::set_params(unsigned long long frequency1,
                                      unsigned long long frequency2,
                                      unsigned long samplerate,
                                      unsigned long bandwidth,
                                      bool quadrature,
                                      bool rfdc,
                                      bool bbdc,
                                      const char* gain1,
                                      double gain1_value,
                                      const char* gain2,
                                      double gain2_value,
                                      const char* gain3,
                                      double gain3_value,
                                      const char* gain4,
                                      double gain4_value,
                                      const char* port_select,
                                      const char* filter_source,
                                      const char* filter_filename,
                                      float Fpass,
                                      float Fstop)
{
    set_params(this->phy,
               frequency1,
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
    set_params(this->phy2,
               frequency2,
               samplerate,
               bandwidth,
               quadrature,
               rfdc,
               bbdc,
               gain3,
               gain3_value,
               gain4,
               gain4_value,
               port_select,
               filter_source,
               filter_filename,
               Fpass,
               Fstop);

    if (this->samplerate != samplerate) {
        ad9361_fmcomms5_multichip_sync(ctx, FIXUP_INTERFACE_TIMING | CHECK_SAMPLE_RATES);
        this->samplerate = samplerate;
    }
}

} /* namespace iio */
} /* namespace gr */
