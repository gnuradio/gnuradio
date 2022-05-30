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

#include "device_source_impl.h"
#include "fmcomms5_sink_impl.h"
#include <gnuradio/blocks/complex_to_float.h>
#include <gnuradio/blocks/float_to_short.h>
#include <gnuradio/io_signature.h>
#include <ad9361.h>

#include <string>
#include <vector>

using namespace gr::blocks;

namespace gr {
namespace iio {

fmcomms5_sink_f32c::fmcomms5_sink_f32c(
    bool tx1_en, bool tx2_en, bool tx3_en, bool tx4_en, fmcomms5_sink::sptr sink_block)
    : hier_block2(
          "fmcomms5_sink_f32c",
          io_signature::make((int)tx1_en + (int)tx2_en + (int)tx3_en + (int)tx4_en,
                             (int)tx1_en + (int)tx2_en + (int)tx3_en + (int)tx4_en,
                             sizeof(gr_complex)),
          io_signature::make(0, 0, 0)),
      fmcomms5_block(sink_block)
{
    basic_block_sptr hier = shared_from_this();
    unsigned int num_streams = (int)tx1_en + (int)tx2_en + (int)tx3_en + (int)tx4_en;

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

fmcomms5_sink::sptr fmcomms5_sink::make(const std::string& uri,
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
                                        bool cyclic,
                                        const char* rf_port_select,
                                        double attenuation1,
                                        double attenuation2,
                                        double attenuation3,
                                        double attenuation4,
                                        const char* filter_source,
                                        const char* filter_filename,
                                        float Fpass,
                                        float Fstop)
{
    return gnuradio::make_block_sptr<fmcomms5_sink_impl>(
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
        cyclic,
        rf_port_select,
        attenuation1,
        attenuation2,
        attenuation3,
        attenuation4,
        filter_source,
        filter_filename,
        Fpass,
        Fstop);
}

fmcomms5_sink::sptr fmcomms5_sink::make_from(iio_context* ctx,
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
                                             bool cyclic,
                                             const char* rf_port_select,
                                             double attenuation1,
                                             double attenuation2,
                                             double attenuation3,
                                             double attenuation4,
                                             const char* filter_source,
                                             const char* filter_filename,
                                             float Fpass,
                                             float Fstop)
{
    return gnuradio::make_block_sptr<fmcomms5_sink_impl>(ctx,
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
                                                         cyclic,
                                                         rf_port_select,
                                                         attenuation1,
                                                         attenuation2,
                                                         attenuation3,
                                                         attenuation4,
                                                         filter_source,
                                                         filter_filename,
                                                         Fpass,
                                                         Fstop);
}

std::vector<std::string> fmcomms5_sink_impl::get_channels_vector(bool ch1_en,
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

fmcomms5_sink_impl::fmcomms5_sink_impl(iio_context* ctx,
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
                                       bool cyclic,
                                       const char* rf_port_select,
                                       double attenuation1,
                                       double attenuation2,
                                       double attenuation3,
                                       double attenuation4,
                                       const char* filter_source,
                                       const char* filter_filename,
                                       float Fpass,
                                       float Fstop)
    : gr::sync_block("fmcomms5_sink",
                     gr::io_signature::make(1, -1, sizeof(short)),
                     gr::io_signature::make(0, 0, 0)),
      device_sink_impl(
          ctx,
          destroy_ctx,
          "cf-ad9361-dds-core-lpc",
          get_channels_vector(
              ch1_en, ch2_en, ch3_en, ch4_en, ch5_en, ch6_en, ch7_en, ch8_en),
          "ad9361-phy",
          std::vector<std::string>(),
          buffer_size,
          0,
          cyclic),
      cyclic(cyclic),
      samplerate(0)
{
    phy2 = iio_context_find_device(ctx, "ad9361-phy-B");
    if (!phy2)
        throw std::runtime_error("Device not found");

    set_params(frequency1,
               frequency2,
               samplerate,
               bandwidth,
               rf_port_select,
               attenuation1,
               attenuation2,
               attenuation3,
               attenuation4,
               filter_source,
               filter_filename,
               Fpass,
               Fstop);
}

void fmcomms5_sink_impl::set_params(iio_device* phy_device,
                                    unsigned long long frequency,
                                    unsigned long samplerate,
                                    unsigned long bandwidth,
                                    const char* rf_port_select,
                                    double attenuation1,
                                    double attenuation2,
                                    const char* filter_source,
                                    const char* filter_filename,
                                    float Fpass,
                                    float Fstop)
{
    iio_param_vec_t params;
    int ret;

    params.emplace_back("out_altvoltage1_TX_LO_frequency", frequency);
    params.emplace_back("out_voltage0_rf_port_select", rf_port_select);
    params.emplace_back("out_voltage0_hardwaregain", -attenuation1);
    params.emplace_back("out_voltage1_hardwaregain", -attenuation2);

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
        if (!filt.empty() && !device_source_impl::load_fir_filter(filt, phy_device))
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

void fmcomms5_sink_impl::set_params(unsigned long long frequency1,
                                    unsigned long long frequency2,
                                    unsigned long samplerate,
                                    unsigned long bandwidth,
                                    const char* rf_port_select,
                                    double attenuation1,
                                    double attenuation2,
                                    double attenuation3,
                                    double attenuation4,
                                    const char* filter_source,
                                    const char* filter_filename,
                                    float Fpass,
                                    float Fstop)
{
    set_params(this->phy,
               frequency1,
               samplerate,
               bandwidth,
               rf_port_select,
               attenuation1,
               attenuation2,
               filter_source,
               filter_filename,
               Fpass,
               Fstop);
    set_params(this->phy2,
               frequency2,
               samplerate,
               bandwidth,
               rf_port_select,
               attenuation3,
               attenuation4,
               filter_source,
               filter_filename,
               Fpass,
               Fstop);

    if (this->samplerate != samplerate) {
        ad9361_fmcomms5_multichip_sync(ctx, FIXUP_INTERFACE_TIMING | CHECK_SAMPLE_RATES);
        this->samplerate = samplerate;
    }
}

int fmcomms5_sink_impl::work(int noutput_items,
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
