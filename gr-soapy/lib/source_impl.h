/* -*- c++ -*- */
/*
 * Copyright 2021 Jeff Long
 * Copyright 2018-2021 Libre Space Foundation <http://libre.space>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_GR_SOAPY_SOURCE_IMPL_H
#define INCLUDED_GR_SOAPY_SOURCE_IMPL_H

#include <functional>

#include "block_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/soapy/source.h>

#include <SoapySDR/Device.hpp>
#include <SoapySDR/Modules.hpp>
#include <SoapySDR/Registry.hpp>
#include <SoapySDR/Version.hpp>

static const pmt::pmt_t TIME_KEY = pmt::string_to_symbol("rx_time");
static const pmt::pmt_t FREQ_KEY = pmt::string_to_symbol("rx_freq");
static const pmt::pmt_t RATE_KEY = pmt::string_to_symbol("rx_rate");

namespace gr {
namespace soapy {

/*!
 * \brief Source block implementation for SDR devices.
 */

class source_impl : public source, public block_impl
{
public:
    source_impl(const std::string& device,
                const std::string& type,
                size_t nchan,
                const std::string& dev_args,
                const std::string& stream_args,
                const std::vector<std::string>& tune_args,
                const std::vector<std::string>& other_settings);
    ~source_impl() override{};

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;

    bool start() override;

    void
    set_frequency(size_t channel, const std::string& name, double frequency) override;
    void set_hardware_time(long long timeNs, const std::string& what) override;
    void set_sample_rate(size_t channel, double sample_rate) override;

private:
    bool _add_tag;
    const bool _has_hardware_time;
};

} // namespace soapy
} // namespace gr

#endif /* INCLUDED_GR_SOAPY_SOURCE_IMPL_H */
