/* -*- c++ -*- */
/*
 * Copyright 2014 Analog Devices Inc.
 * Author: Paul Cercueil <paul.cercueil@analog.com>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cstdio>

#include "device_source_impl.h"
#include "fmcomms2_sink_impl.h"
#include <gnuradio/io_signature.h>

#include <gnuradio/blocks/complex_to_float.h>
#include <gnuradio/blocks/float_to_short.h>

#include <ad9361.h>

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
                                        unsigned long long frequency,
                                        unsigned long samplerate,
                                        unsigned long bandwidth,
                                        bool ch1_en,
                                        bool ch2_en,
                                        bool ch3_en,
                                        bool ch4_en,
                                        unsigned long buffer_size,
                                        bool cyclic,
                                        const char* rf_port_select,
                                        double attenuation1,
                                        double attenuation2,
                                        const char* filter_source,
                                        const char* filter_filename,
                                        float Fpass,
                                        float Fstop)
{
    return gnuradio::get_initial_sptr(
        new fmcomms2_sink_impl(device_source_impl::get_context(uri),
                               true,
                               frequency,
                               samplerate,
                               bandwidth,
                               ch1_en,
                               ch2_en,
                               ch3_en,
                               ch4_en,
                               buffer_size,
                               cyclic,
                               rf_port_select,
                               attenuation1,
                               attenuation2,
                               filter_source,
                               filter_filename,
                               Fpass,
                               Fstop));
}

fmcomms2_sink::sptr fmcomms2_sink::make_from(struct iio_context* ctx,
                                             unsigned long long frequency,
                                             unsigned long samplerate,
                                             unsigned long bandwidth,
                                             bool ch1_en,
                                             bool ch2_en,
                                             bool ch3_en,
                                             bool ch4_en,
                                             unsigned long buffer_size,
                                             bool cyclic,
                                             const char* rf_port_select,
                                             double attenuation1,
                                             double attenuation2,
                                             const char* filter_source,
                                             const char* filter_filename,
                                             float Fpass,
                                             float Fstop)
{
    return gnuradio::get_initial_sptr(new fmcomms2_sink_impl(ctx,
                                                             false,
                                                             frequency,
                                                             samplerate,
                                                             bandwidth,
                                                             ch1_en,
                                                             ch2_en,
                                                             ch3_en,
                                                             ch4_en,
                                                             buffer_size,
                                                             cyclic,
                                                             rf_port_select,
                                                             attenuation1,
                                                             attenuation2,
                                                             filter_source,
                                                             filter_filename,
                                                             Fpass,
                                                             Fstop));
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

fmcomms2_sink_impl::fmcomms2_sink_impl(struct iio_context* ctx,
                                       bool destroy_ctx,
                                       unsigned long long frequency,
                                       unsigned long samplerate,
                                       unsigned long bandwidth,
                                       bool ch1_en,
                                       bool ch2_en,
                                       bool ch3_en,
                                       bool ch4_en,
                                       unsigned long buffer_size,
                                       bool cyclic,
                                       const char* rf_port_select,
                                       double attenuation1,
                                       double attenuation2,
                                       const char* filter_source,
                                       const char* filter_filename,
                                       float Fpass,
                                       float Fstop)
    : gr::sync_block("fmcomms2_sink",
                     gr::io_signature::make(1, -1, sizeof(short)),
                     gr::io_signature::make(0, 0, 0)),
      device_sink_impl(ctx,
                       destroy_ctx,
                       "cf-ad9361-dds-core-lpc",
                       get_channels_vector(ch1_en, ch2_en, ch3_en, ch4_en),
                       "ad9361-phy",
                       std::vector<std::string>(),
                       buffer_size,
                       0,
                       cyclic),
      cyclic(cyclic)
{
    set_params(frequency,
               samplerate,
               bandwidth,
               rf_port_select,
               attenuation1,
               attenuation2,
               filter_source,
               filter_filename,
               Fpass,
               Fstop);

    stop_thread = false;
    underflow_thd = boost::thread(&fmcomms2_sink_impl::check_underflow, this);
}

fmcomms2_sink_impl::~fmcomms2_sink_impl()
{
    boost::unique_lock<boost::mutex> lock(uf_mutex);
    stop_thread = true;
    lock.unlock();
    underflow_thd.join();
}

void fmcomms2_sink_impl::check_underflow(void)
{
    uint32_t status;
    int ret;
    boost::unique_lock<boost::mutex> lock(uf_mutex, boost::defer_lock);

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

void fmcomms2_sink_impl::set_params(unsigned long long frequency,
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
    bool is_fmcomms4 = !iio_device_find_channel(phy, "voltage1", false);
    std::vector<std::string> params;

    bool auto_filter = false;
    if (filter_filename && filter_filename[0])
        auto_filter = false;

    params.push_back("out_altvoltage1_TX_LO_frequency=" + boost::to_string(frequency));
    if (!auto_filter) {
        params.push_back("out_voltage_sampling_frequency=" +
                         boost::to_string(samplerate));
    }
    params.push_back("out_voltage_rf_bandwidth=" + boost::to_string(bandwidth));
    params.push_back("out_voltage0_rf_port_select=" + boost::to_string(rf_port_select));
    params.push_back("out_voltage0_hardwaregain=" + boost::to_string(-attenuation1));

    if (!is_fmcomms4) {
        params.push_back("out_voltage1_hardwaregain=" + boost::to_string(-attenuation2));
    }

    device_source_impl::set_params(this->phy, params);

    if (auto_filter) {
        int ret = ad9361_set_bb_rate(phy, samplerate);
        if (ret) {
            throw std::runtime_error("Unable to set BB rate");
        }
    } else if (filter_filename && filter_filename[0]) {
        std::string f(filter_filename);
        if (!device_source_impl::load_fir_filter(f, phy))
            throw std::runtime_error("Unable to load filter file");
    }
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
