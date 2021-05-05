/* -*- c++ -*- */
/*
 * Copyright 2014 Analog Devices Inc.
 * Author: Paul Cercueil <paul.cercueil@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_IIO_FMCOMMS2_SOURCE_IMPL_H
#define INCLUDED_IIO_FMCOMMS2_SOURCE_IMPL_H

#include "device_source_impl.h"
#include <gnuradio/iio/fmcomms2_source.h>

#include <string>
#include <thread>
#include <vector>

namespace gr {
namespace iio {

class fmcomms2_source_impl : public fmcomms2_source, public device_source_impl
{
private:
    std::vector<std::string>
    get_channels_vector(bool ch1_en, bool ch2_en, bool ch3_en, bool ch4_en);
    std::vector<std::string> get_channels_vector(const std::vector<bool>& ch_en);
    std::thread overflow_thd;
    void check_overflow(void);

public:
    // fmcomms2_source_impl(struct iio_context* ctx,
    //                      bool destroy_ctx,
    //                      unsigned long long frequency,
    //                      unsigned long samplerate,
    //                      unsigned long bandwidth,
    //                      bool ch1_en,
    //                      bool ch2_en,
    //                      bool ch3_en,
    //                      bool ch4_en,
    //                      unsigned long buffer_size,
    //                      bool quadrature,
    //                      bool rfdc,
    //                      bool bbdc,
    //                      const char* gain1,
    //                      double gain1_value,
    //                      const char* gain2,
    //                      double gain2_value,
    //                      const char* rf_port_select,
    //                      const char* filter_source,
    //                      const char* filter_filename,
    //                      float Fpass,
    //                      float Fstop);
    fmcomms2_source_impl(struct iio_context* ctx,
                         const std::vector<bool>& ch_en,
                         unsigned long buffer_size);

    ~fmcomms2_source_impl();

    // void set_params(unsigned long long frequency,
    //                 unsigned long samplerate,
    //                 unsigned long bandwidth,
    //                 bool quadrature,
    //                 bool rfdc,
    //                 bool bbdc,
    //                 const char* gain1,
    //                 double gain1_value,
    //                 const char* gain2,
    //                 double gain2_value,
    //                 const char* rf_port_select,
    //                 const char* filter_source,
    //                 const char* filter_filename,
    //                 float Fpass,
    //                 float Fstop);

    virtual void set_frequency(unsigned long long frequency);
    virtual void set_samplerate(unsigned long samplerate);
    virtual void set_gain_mode(size_t chan, const std::string& mode);
    virtual void set_gain(size_t chan, double gain_value);

protected:
    unsigned long long d_frequency = 2400000000;
    unsigned long d_samplerate = 1000000;
    unsigned long d_bandwidth = 20000000;
    bool d_quadrature = true;
    bool d_rfdc = true;
    bool d_bbdc = true;
    std::vector<std::string> d_gain_mode = {
        "manual", "manual", "manual", "manual"
    }; // TODO - make these enums
    std::vector<double> d_gain_value = { 0, 0, 0, 0 };
    std::string d_rf_port_select = "A_BALANCED";
    std::string d_filter_source = "Auto";
    std::string d_filter_filename = "";
    float d_fpass = (float)d_samplerate / 4.0;
    float d_fstop = (float)d_samplerate / 3.0;
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_FMCOMMS2_SOURCE_IMPL_H */
