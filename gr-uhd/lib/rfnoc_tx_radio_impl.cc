/* -*- c++ -*- */
/*
 * Copyright 2019 Ettus Research, a National Instruments Brand.
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "rfnoc_tx_radio_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace uhd {

constexpr size_t MAX_RADIO_REFS = 2; // One for RX, one for TX

rfnoc_tx_radio::sptr rfnoc_tx_radio::make(rfnoc_graph::sptr graph,
                                          const ::uhd::device_addr_t& block_args,
                                          const int device_select,
                                          const int instance)
{
    return gnuradio::make_block_sptr<rfnoc_tx_radio_impl>(rfnoc_block::make_block_ref(
        graph, block_args, "Radio", device_select, instance, MAX_RADIO_REFS));
}


rfnoc_tx_radio_impl::rfnoc_tx_radio_impl(::uhd::rfnoc::noc_block_base::sptr block_ref)
    : rfnoc_block(block_ref),
      d_wrapped_ref(std::dynamic_pointer_cast<::uhd::rfnoc::radio_control>(block_ref))
{
}

rfnoc_tx_radio_impl::~rfnoc_tx_radio_impl() {}

/******************************************************************************
 * rfnoc_tx_radio API
 *****************************************************************************/
double rfnoc_tx_radio_impl::set_rate(const double rate)
{
    return d_wrapped_ref->set_rate(rate);
}

void rfnoc_tx_radio_impl::set_antenna(const std::string& antenna, const size_t chan)
{
    return d_wrapped_ref->set_tx_antenna(antenna, chan);
}

double rfnoc_tx_radio_impl::set_frequency(const double frequency, const size_t chan)
{
    return d_wrapped_ref->set_tx_frequency(frequency, chan);
}

void rfnoc_tx_radio_impl::set_tune_args(const ::uhd::device_addr_t& args,
                                        const size_t chan)
{
    return d_wrapped_ref->set_tx_tune_args(args, chan);
}

double rfnoc_tx_radio_impl::set_gain(const double gain, const size_t chan)
{
    return d_wrapped_ref->set_tx_gain(gain, chan);
}

double rfnoc_tx_radio_impl::set_gain(const double gain,
                                     const std::string& name,
                                     const size_t chan)
{
    return d_wrapped_ref->set_tx_gain(gain, name, chan);
}

void rfnoc_tx_radio_impl::set_gain_profile(const std::string& profile, const size_t chan)
{
    return d_wrapped_ref->set_tx_gain_profile(profile, chan);
}

double rfnoc_tx_radio_impl::set_bandwidth(const double bandwidth, const size_t chan)
{
    return d_wrapped_ref->set_tx_bandwidth(bandwidth, chan);
}

void rfnoc_tx_radio_impl::set_lo_source(const std::string& source,
                                        const std::string& name,
                                        const size_t chan)
{
    return d_wrapped_ref->set_tx_lo_source(source, name, chan);
}

void rfnoc_tx_radio_impl::set_lo_export_enabled(const bool enabled,
                                                const std::string& name,
                                                const size_t chan)
{
    return d_wrapped_ref->set_tx_lo_export_enabled(enabled, name, chan);
}

double rfnoc_tx_radio_impl::set_lo_freq(const double freq,
                                        const std::string& name,
                                        const size_t chan)
{
    return d_wrapped_ref->set_tx_lo_freq(freq, name, chan);
}

void rfnoc_tx_radio_impl::set_dc_offset(const std::complex<double>& offset,
                                        const size_t chan)
{
    return d_wrapped_ref->set_tx_dc_offset(offset, chan);
}

void rfnoc_tx_radio_impl::set_iq_balance(const std::complex<double>& correction,
                                         const size_t chan)
{
    return d_wrapped_ref->set_tx_iq_balance(correction, chan);
}

} /* namespace uhd */
} /* namespace gr */
