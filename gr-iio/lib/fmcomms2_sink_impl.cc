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

#include <mutex>
#include <string>
#include <thread>
#include <vector>

#define MIN_RATE 520333
#define DECINT_RATIO 8
#define OVERFLOW_CHECK_PERIOD_MS 1000

namespace gr {
namespace iio {

template <typename T>
typename fmcomms2_sink<T>::sptr fmcomms2_sink<T>::make(const std::string& uri,
                                                       const std::vector<bool>& ch_en,
                                                       unsigned long buffer_size,
                                                       bool cyclic)
{
    return gnuradio::make_block_sptr<fmcomms2_sink_impl<T>>(
        device_source_impl::get_context(uri), ch_en, buffer_size, cyclic);
}

template <typename T>
std::vector<std::string> fmcomms2_sink_impl<T>::get_channels_vector(bool ch1_en,
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

template <typename T>
std::vector<std::string>
fmcomms2_sink_impl<T>::get_channels_vector(const std::vector<bool>& ch_en)
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

template <>
fmcomms2_sink_impl<int16_t>::fmcomms2_sink_impl(iio_context* ctx,
                                                const std::vector<bool>& ch_en,
                                                unsigned long buffer_size,
                                                bool cyclic)
    : gr::sync_block("fmcomms2_sink",
                     gr::io_signature::make(1, -1, sizeof(int16_t)),
                     gr::io_signature::make(0, 0, 0)),
      device_sink_impl(ctx,
                       true,
                       "cf-ad9361-dds-core-lpc",
                       get_channels_vector(ch_en),
                       "ad9361-phy",
                       iio_param_vec_t(),
                       buffer_size,
                       0,
                       cyclic),
      cyclic(cyclic)
{
    stop_thread = false;
    underflow_thd = std::thread(&fmcomms2_sink_impl<int16_t>::check_underflow, this);
}

template <typename T>
fmcomms2_sink_impl<T>::fmcomms2_sink_impl(iio_context* ctx,
                                          const std::vector<bool>& ch_en,
                                          unsigned long buffer_size,
                                          bool cyclic)
    : gr::sync_block("fmcomms2_sink",
                     gr::io_signature::make(1, -1, sizeof(T)),
                     gr::io_signature::make(0, 0, 0)),
      device_sink_impl(ctx,
                       true,
                       "cf-ad9361-dds-core-lpc",
                       get_channels_vector(ch_en),
                       "ad9361-phy",
                       iio_param_vec_t(),
                       buffer_size,
                       0,
                       cyclic),
      cyclic(cyclic)
{
    stop_thread = false;
    underflow_thd = std::thread(&fmcomms2_sink_impl<T>::check_underflow, this);

    // Device Buffers are always presented as short from device_sink
    auto nchans = get_channels_vector(ch_en).size();
    d_device_bufs.resize(nchans);
    d_device_item_ptrs.resize(nchans);
    for (size_t i = 0; i < d_device_bufs.size(); i++) {
        d_device_bufs[i].resize(s_initial_device_buf_size);
    }
    d_float_r.resize(s_initial_device_buf_size);
    d_float_i.resize(s_initial_device_buf_size);

    // Tell tagger in device_sink_impl::work that we are using a less inputs
    override_tagged_input_channels = d_device_bufs.size() / 2;
}

template <typename T>
fmcomms2_sink_impl<T>::~fmcomms2_sink_impl()
{
    std::unique_lock<std::mutex> lock(uf_mutex);
    stop_thread = true;
    lock.unlock();
    underflow_thd.join();
}

template <typename T>
void fmcomms2_sink_impl<T>::set_len_tag_key(const std::string& str)
{
    device_sink_impl::set_len_tag_key(str);
}

template <typename T>
void fmcomms2_sink_impl<T>::check_underflow(void)
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

template <typename T>
void fmcomms2_sink_impl<T>::set_bandwidth(unsigned long bandwidth)
{
    iio_param_vec_t params;
    params.emplace_back("out_voltage_rf_bandwidth", bandwidth);
    device_source_impl::set_params(this->phy, params);
    d_bandwidth = bandwidth;
}

template <typename T>
void fmcomms2_sink_impl<T>::set_rf_port_select(const std::string& rf_port_select)
{
    iio_param_vec_t params;
    params.emplace_back("out_voltage0_rf_port_select", rf_port_select);
    device_source_impl::set_params(this->phy, params);
    d_rf_port_select = rf_port_select;
}

template <typename T>
void fmcomms2_sink_impl<T>::set_frequency(double frequency)
{
    iio_param_vec_t params;
    params.emplace_back("out_altvoltage1_TX_LO_frequency",
                        static_cast<unsigned long long>(frequency));
    device_source_impl::set_params(this->phy, params);
    d_frequency = frequency;
}

template <typename T>
void fmcomms2_sink_impl<T>::set_samplerate(unsigned long samplerate)
{
    iio_param_vec_t params;
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

template <typename T>
void fmcomms2_sink_impl<T>::set_attenuation(size_t chan, double attenuation)
{
    bool is_fmcomms4 = !iio_device_find_channel(phy, "voltage1", false);
    if ((is_fmcomms4 && chan > 0) || chan > 1) {
        throw std::runtime_error("Channel out of range for this device");
    }
    iio_param_vec_t params;
    params.emplace_back("out_voltage" + std::to_string(chan) + "_hardwaregain",
                        -attenuation);
    device_source_impl::set_params(this->phy, params);

    d_attenuation[chan] = attenuation;
}

template <typename T>
void fmcomms2_sink_impl<T>::update_dependent_params()
{
    iio_param_vec_t params;
    // Set rate configuration
    if (d_filter_source.compare("Off") == 0) {
        params.emplace_back("out_voltage_sampling_frequency", d_samplerate);
        params.emplace_back("out_voltage_rf_bandwidth", d_bandwidth);
    } else if (d_filter_source.compare("Auto") == 0) {
        int ret = ad9361_set_bb_rate(phy, d_samplerate);
        if (ret) {
            throw std::runtime_error("Unable to set BB rate");
            params.emplace_back("out_voltage_rf_bandwidth", d_bandwidth);
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

template <typename T>
void fmcomms2_sink_impl<T>::set_filter_params(const std::string& filter_source,
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

template <>
int fmcomms2_sink_impl<std::int16_t>::work(int noutput_items,
                                           gr_vector_const_void_star& input_items,
                                           gr_vector_void_star& output_items)
{
    // For int16_t ports, each sc16 port gets mapped into 2 device channels
    if (2 * input_items.size() > d_device_item_ptrs.size()) {
        d_device_item_ptrs.resize(2 * input_items.size());
        d_device_bufs.resize(2 * input_items.size());
    }

    for (size_t i = 0; i < input_items.size(); i++) {
        auto in = static_cast<const std::complex<int16_t>*>(input_items[i]);
        if (noutput_items > (int)d_device_bufs[i].size()) {
            d_device_bufs[2 * i].resize(noutput_items);
            d_device_bufs[2 * i + 1].resize(noutput_items);
        }
        d_device_item_ptrs[2 * i] = static_cast<const void*>(d_device_bufs[2 * i].data());
        d_device_item_ptrs[2 * i + 1] =
            static_cast<const void*>(d_device_bufs[2 * i + 1].data());

        // deinterleave complex to float
        volk_16ic_deinterleave_16i_x2(d_device_bufs[2 * i].data(),
                                      d_device_bufs[2 * i + 1].data(),
                                      in,
                                      noutput_items);
    }

    int ret = device_sink_impl::work(noutput_items, d_device_item_ptrs, output_items);
    if (ret < 0 || !cyclic)
        return ret;
    else
        return WORK_DONE;
}

template <>
int fmcomms2_sink_impl<gr_complex>::work(int noutput_items,
                                         gr_vector_const_void_star& input_items,
                                         gr_vector_void_star& output_items)
{
    // For gr_complex ports, each gr_complex port gets mapped into 2 device channels
    if (2 * input_items.size() > d_device_item_ptrs.size()) {
        d_device_item_ptrs.resize(2 * input_items.size());
        d_device_bufs.resize(2 * input_items.size());
    }

    for (size_t i = 0; i < input_items.size(); i++) {
        auto in = static_cast<const gr_complex*>(input_items[i]);
        if (noutput_items > (int)d_device_bufs[2 * i].size()) {
            d_device_bufs[2 * i].resize(noutput_items);
            d_device_bufs[2 * i + 1].resize(noutput_items);
            d_float_r.resize(noutput_items);
            d_float_i.resize(noutput_items);
        }
        d_device_item_ptrs[2 * i] = static_cast<const void*>(d_device_bufs[2 * i].data());
        d_device_item_ptrs[2 * i + 1] =
            static_cast<const void*>(d_device_bufs[2 * i + 1].data());


        // deinterleave complex to float
        volk_32fc_deinterleave_32f_x2(
            d_float_r.data(), d_float_i.data(), in, noutput_items);
        // float to short
        volk_32f_s32f_convert_16i(
            d_device_bufs[2 * i].data(), d_float_r.data(), 32768.0, noutput_items);
        volk_32f_s32f_convert_16i(
            d_device_bufs[2 * i + 1].data(), d_float_i.data(), 32768.0, noutput_items);
    }


    int ret = device_sink_impl::work(noutput_items, d_device_item_ptrs, output_items);
    if (ret < 0 || !cyclic)
        return ret;
    else
        return WORK_DONE;
}

template <typename T>
int fmcomms2_sink_impl<T>::work(int noutput_items,
                                gr_vector_const_void_star& input_items,
                                gr_vector_void_star& output_items)
{
    int ret = device_sink_impl::work(noutput_items, input_items, output_items);
    if (ret < 0 || !cyclic)
        return ret;
    else
        return WORK_DONE;
}

template class fmcomms2_sink<int16_t>;
template class fmcomms2_sink<std::complex<int16_t>>;
template class fmcomms2_sink<gr_complex>;

} /* namespace iio */
} /* namespace gr */
