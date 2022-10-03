/* -*- c++ -*- */
/*
 * Copyright 2022 Ettus Research, A National Instruments Brand.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "rfnoc_siggen_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace uhd {

namespace {

::uhd::rfnoc::siggen_waveform waveform_type_gr2u(rfnoc_siggen::siggen_waveform wft)
{
    return static_cast<::uhd::rfnoc::siggen_waveform>(wft);
}

rfnoc_siggen::siggen_waveform waveform_type_u2gr(::uhd::rfnoc::siggen_waveform wft)
{
    return static_cast<rfnoc_siggen::siggen_waveform>(wft);
}

} // namespace

rfnoc_siggen::sptr rfnoc_siggen::make(rfnoc_graph::sptr graph,
                                      const ::uhd::device_addr_t& block_args,
                                      const int device_select,
                                      const int instance)
{
    return gnuradio::make_block_sptr<rfnoc_siggen_impl>(rfnoc_block::make_block_ref(
        graph, block_args, "SigGen", device_select, instance));
}


rfnoc_siggen_impl::rfnoc_siggen_impl(::uhd::rfnoc::noc_block_base::sptr block_ref)
    : rfnoc_block(block_ref),
      d_siggen_ref(get_block_ref<::uhd::rfnoc::siggen_block_control>())
{
}

rfnoc_siggen_impl::~rfnoc_siggen_impl() {}

/******************************************************************************
 * rfnoc_siggen API
 *****************************************************************************/
void rfnoc_siggen_impl::set_amplitude(const double amplitude, const size_t chan = 0)
{
    d_siggen_ref->set_amplitude(amplitude, chan);
}

double rfnoc_siggen_impl::get_amplitude(const size_t chan = 0)
{
    return d_siggen_ref->get_amplitude(chan);
}

void rfnoc_siggen_impl::set_constant(const std::complex<double> constant,
                                     const size_t chan = 0)
{
    d_siggen_ref->set_constant(constant, chan);
}

std::complex<double> rfnoc_siggen_impl::get_constant(const size_t chan = 0)
{
    return d_siggen_ref->get_constant(chan);
}

void rfnoc_siggen_impl::set_enable(const bool enable, const size_t chan = 0)
{
    d_siggen_ref->set_enable(enable, chan);
}

bool rfnoc_siggen_impl::get_enable(const size_t chan = 0)
{
    return d_siggen_ref->get_enable(chan);
}

void rfnoc_siggen_impl::set_sine_phase_increment(const double phase_inc,
                                                 const size_t chan = 0)
{
    d_siggen_ref->set_sine_phase_increment(phase_inc, chan);
}

double rfnoc_siggen_impl::get_sine_phase_increment(const size_t chan = 0)
{
    return d_siggen_ref->get_sine_phase_increment(chan);
}

void rfnoc_siggen_impl::set_sine_frequency(const double frequency,
                                           const double sample_rate,
                                           const size_t chan = 0)
{
    d_siggen_ref->set_sine_frequency(frequency, sample_rate, chan);
}

void rfnoc_siggen_impl::set_waveform(const rfnoc_siggen::siggen_waveform type,
                                     const size_t chan = 0)
{
    d_siggen_ref->set_waveform(waveform_type_gr2u(type), chan);
}

void rfnoc_siggen_impl::set_waveform(const std::string& type, const size_t chan = 0)
{
    if (type == "NOISE") {
        d_siggen_ref->set_waveform(::uhd::rfnoc::siggen_waveform::NOISE, chan);
    } else if (type == "CONSTANT") {
        d_siggen_ref->set_waveform(::uhd::rfnoc::siggen_waveform::CONSTANT, chan);
    } else if (type == "SINE_WAVE") {
        d_siggen_ref->set_waveform(::uhd::rfnoc::siggen_waveform::SINE_WAVE, chan);
    } else {
        throw std::runtime_error(fmt::format(
            "SigGen waveform type ({:s}) is not [NOISE, CONSTANT, SINE_WAVE]", type));
    }
}

rfnoc_siggen::siggen_waveform rfnoc_siggen_impl::get_waveform(const size_t chan)
{
    return waveform_type_u2gr(d_siggen_ref->get_waveform(chan));
}

std::string rfnoc_siggen_impl::get_waveform_string(const size_t chan)
{
    const auto type = get_waveform(chan);
    switch (type) {
    case siggen_waveform::NOISE:
        return "NOISE";
    case siggen_waveform::CONSTANT:
        return "CONSTANT";
    case siggen_waveform::SINE_WAVE:
        return "SINE_WAVE";
    default:
        throw std::runtime_error(fmt::format(
            "SigGen waveform type enumeration ({:d}) is not [NOISE, CONSTANT, SINE_WAVE]",
            static_cast<uint32_t>(type)));
    }
}

void rfnoc_siggen_impl::set_samples_per_packet(const size_t spp, const size_t chan = 0)
{
    d_siggen_ref->set_samples_per_packet(spp, chan);
}

size_t rfnoc_siggen_impl::get_samples_per_packet(const size_t chan = 0)
{
    return d_siggen_ref->get_samples_per_packet(chan);
}

} // namespace uhd
} // namespace gr
