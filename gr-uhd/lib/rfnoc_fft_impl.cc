/* -*- c++ -*- */
/*
 * Copyright 2022 Ettus Research, A National Instruments Brand.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "rfnoc_fft_impl.h"
#include <gnuradio/io_signature.h>
#include <uhd/version.hpp>

namespace gr {
namespace uhd {

namespace {
// Convert from GNU Radio to UHD
::uhd::rfnoc::fft_direction fft_direction_gr2u(rfnoc_fft::fft_direction dir)
{
    return static_cast<::uhd::rfnoc::fft_direction>(dir);
}

rfnoc_fft::fft_direction fft_direction_u2gr(::uhd::rfnoc::fft_direction dir)
{
    return static_cast<rfnoc_fft::fft_direction>(dir);
}

::uhd::rfnoc::fft_magnitude fft_magnitude_gr2u(rfnoc_fft::fft_magnitude mag)
{
    return static_cast<::uhd::rfnoc::fft_magnitude>(mag);
}

rfnoc_fft::fft_magnitude fft_magnitude_u2gr(::uhd::rfnoc::fft_magnitude mag)
{
    return static_cast<rfnoc_fft::fft_magnitude>(mag);
}

::uhd::rfnoc::fft_shift fft_shift_gr2u(rfnoc_fft::fft_shift shift)
{
    return static_cast<::uhd::rfnoc::fft_shift>(shift);
}

rfnoc_fft::fft_shift fft_shift_u2gr(::uhd::rfnoc::fft_shift shift)
{
    return static_cast<rfnoc_fft::fft_shift>(shift);
}
} // namespace

rfnoc_fft::sptr rfnoc_fft::make(rfnoc_graph::sptr graph,
                                const ::uhd::device_addr_t& block_args,
                                const int device_select,
                                const int instance)
{
    return gnuradio::make_block_sptr<rfnoc_fft_impl>(
        rfnoc_block::make_block_ref(graph, block_args, "FFT", device_select, instance));
}


rfnoc_fft_impl::rfnoc_fft_impl(::uhd::rfnoc::noc_block_base::sptr block_ref)
    : rfnoc_block(block_ref), d_fft_ref(get_block_ref<::uhd::rfnoc::fft_block_control>())
{
}

rfnoc_fft_impl::~rfnoc_fft_impl() {}

/******************************************************************************
 * rfnoc_fft API
 *****************************************************************************/


// FFT Direction
void rfnoc_fft_impl::set_direction(const rfnoc_fft::fft_direction direction)
{
    d_fft_ref->set_direction(fft_direction_gr2u(direction));
}

void rfnoc_fft_impl::set_direction(const std::string& direction)
{
    if (direction == "REVERSE") {
        d_fft_ref->set_direction(::uhd::rfnoc::fft_direction::REVERSE);
    } else if (direction == "FORWARD") {
        d_fft_ref->set_direction(::uhd::rfnoc::fft_direction::FORWARD);
    } else {
        throw std::runtime_error(
            fmt::format("FFT direction ({:s}) is not [REVERSE, FORWARD]", direction));
    }
}

rfnoc_fft::fft_direction rfnoc_fft_impl::get_direction() const
{
    return fft_direction_u2gr(d_fft_ref->get_direction());
}

std::string rfnoc_fft_impl::get_direction_string() const
{
    const auto direction = get_direction();
    switch (direction) {
    case rfnoc_fft::fft_direction::REVERSE:
        return "REVERSE";
    case rfnoc_fft::fft_direction::FORWARD:
        return "FORWARD";
    default:
        throw std::runtime_error(
            fmt::format("FFT direction enumeration ({:d}) is not [REVERSE, FORWARD]",
                        static_cast<uint32_t>(direction)));
    }
}

// FFT Shift Configuration
void rfnoc_fft_impl::set_shift_config(const rfnoc_fft::fft_shift shift)
{
    d_fft_ref->set_shift_config(fft_shift_gr2u(shift));
}

void rfnoc_fft_impl::set_shift_config(const std::string& shift)
{
    if (shift == "NORMAL") {
        d_fft_ref->set_shift_config(::uhd::rfnoc::fft_shift::NORMAL);
    } else if (shift == "REVERSE") {
        d_fft_ref->set_shift_config(::uhd::rfnoc::fft_shift::REVERSE);
    } else if (shift == "NATURAL") {
        d_fft_ref->set_shift_config(::uhd::rfnoc::fft_shift::NATURAL);
    } else if (shift == "BIT_REVERSE") {
#if UHD_VERSION >= 4080000
        d_fft_ref->set_shift_config(::uhd::rfnoc::fft_shift::BIT_REVERSE);
#else
        throw std::runtime_error(
            "BIT_REVERSE shift configuration is not supported in this UHD version");
#endif
    } else {
        throw std::runtime_error(fmt::format("FFT shift configuration ({:s}) is not "
                                             "[NORMAL, REVERSE, NATURAL, BIT_REVERSE]",
                                             shift));
    }
}

rfnoc_fft::fft_shift rfnoc_fft_impl::get_shift_config() const
{
    return fft_shift_u2gr(d_fft_ref->get_shift_config());
}

std::string rfnoc_fft_impl::get_shift_config_string() const
{
    const auto shift = get_shift_config();
    switch (shift) {
    case rfnoc_fft::fft_shift::NORMAL:
        return "NORMAL";
    case rfnoc_fft::fft_shift::REVERSE:
        return "REVERSE";
    case rfnoc_fft::fft_shift::NATURAL:
        return "NATURAL";
    case rfnoc_fft::fft_shift::BIT_REVERSE:
        return "BIT_REVERSE";
    default:
        throw std::runtime_error(
            fmt::format("FFT shift configuration enumeration ({:d}) is not [NORMAL, "
                        "REVERSE, NATURAL, BIT_REVERSE]",
                        static_cast<uint32_t>(shift)));
    }
}


// FFT Magnitude
void rfnoc_fft_impl::set_magnitude(const rfnoc_fft::fft_magnitude magnitude)
{
    d_fft_ref->set_magnitude(fft_magnitude_gr2u(magnitude));
}

void rfnoc_fft_impl::set_magnitude(const std::string& magnitude)
{
    if (magnitude == "COMPLEX") {
        d_fft_ref->set_magnitude(::uhd::rfnoc::fft_magnitude::COMPLEX);
    } else if (magnitude == "MAGNITUDE") {
        d_fft_ref->set_magnitude(::uhd::rfnoc::fft_magnitude::MAGNITUDE);
    } else if (magnitude == "MAGNITUDE_SQUARED") {
        d_fft_ref->set_magnitude(::uhd::rfnoc::fft_magnitude::MAGNITUDE_SQUARED);
    } else {
        throw std::runtime_error(fmt::format(
            "FFT magnitude ({:s}) is not [COMPLEX, MAGNITUDE, MAGNITUDE_SQUARED]",
            magnitude));
    }
}
rfnoc_fft::fft_magnitude rfnoc_fft_impl::get_magnitude() const
{
    return fft_magnitude_u2gr(d_fft_ref->get_magnitude());
}

std::string rfnoc_fft_impl::get_magnitude_string() const
{
    const auto magnitude = get_magnitude();
    switch (magnitude) {
    case rfnoc_fft::fft_magnitude::COMPLEX:
        return "COMPLEX";
    case rfnoc_fft::fft_magnitude::MAGNITUDE:
        return "MAGNITUDE";
    case rfnoc_fft::fft_magnitude::MAGNITUDE_SQUARED:
        return "MAGNITUDE_SQUARED";
    default:
        throw std::runtime_error(fmt::format("FFT magnitude enumeration ({:d}) is not "
                                             "[COMPLEX, MAGNITUDE, MAGNITUDE_SQUARED]",
                                             static_cast<uint32_t>(magnitude)));
    }
}

// FFT Scaling
void rfnoc_fft_impl::set_scaling(const uint32_t scaling)
{
    d_fft_ref->set_scaling(scaling);
}

uint32_t rfnoc_fft_impl::get_scaling() const { return d_fft_ref->get_scaling(); }

// FFT Scaling Factor
void rfnoc_fft_impl::set_scaling_factor(const double factor)
{
#if UHD_VERSION >= 4080000
    d_fft_ref->set_scaling_factor(factor);
#else
    throw std::runtime_error("set_scaling_factor is not supported in this UHD version");
#endif
}

// FFT Length
void rfnoc_fft_impl::set_length(const uint32_t length) { d_fft_ref->set_length(length); }

uint32_t rfnoc_fft_impl::get_length() const { return d_fft_ref->get_length(); }

// FFT Bypass Mode
void rfnoc_fft_impl::set_bypass_mode(const bool bypass)
{
#if UHD_VERSION >= 4080000
    d_fft_ref->set_bypass_mode(bypass);
#else
    throw std::runtime_error("set_bypass_mode is not supported in this UHD version");
#endif
}

bool rfnoc_fft_impl::get_bypass_mode() const
{
#if UHD_VERSION >= 4080000
    return d_fft_ref->get_bypass_mode();
#else
    throw std::runtime_error("get_bypass_mode is not supported in this UHD version");
#endif
}

// NIPC
uint32_t rfnoc_fft_impl::get_nipc() const
{
#if UHD_VERSION >= 4080000
    return d_fft_ref->get_nipc();
#else
    throw std::runtime_error("get_nipc is not supported in this UHD version");
#endif
}

// Max Length
uint32_t rfnoc_fft_impl::get_max_length() const
{
#if UHD_VERSION >= 4080000
    return d_fft_ref->get_max_length();
#else
    throw std::runtime_error("get_max_length is not supported in this UHD version");
#endif
}

// Max CP Length
uint32_t rfnoc_fft_impl::get_max_cp_length() const
{
#if UHD_VERSION >= 4080000
    return d_fft_ref->get_max_cp_length();
#else
    throw std::runtime_error("get_max_cp_length is not supported in this UHD version");
#endif
}

// Max CP Removal List Length
uint32_t rfnoc_fft_impl::get_max_cp_removal_list_length() const
{
#if UHD_VERSION >= 4080000
    return d_fft_ref->get_max_cp_removal_list_length();
#else
    throw std::runtime_error(
        "get_max_cp_removal_list_length is not supported in this UHD version");
#endif
}

// Max CP Insertion List Length
uint32_t rfnoc_fft_impl::get_max_cp_insertion_list_length() const
{
#if UHD_VERSION >= 4080000
    return d_fft_ref->get_max_cp_insertion_list_length();
#else
    throw std::runtime_error(
        "get_max_cp_insertion_list_length is not supported in this UHD version");
#endif
}

// CP Insertion List
void rfnoc_fft_impl::set_cp_insertion_list(const std::vector<uint32_t> cp_lengths)
{
#if UHD_VERSION >= 4080000
    d_fft_ref->set_cp_insertion_list(cp_lengths);
#else
    throw std::runtime_error(
        "set_cp_insertion_list is not supported in this UHD version");
#endif
}

std::vector<uint32_t> rfnoc_fft_impl::get_cp_insertion_list() const
{
#if UHD_VERSION >= 4080000
    return d_fft_ref->get_cp_insertion_list();
#else
    throw std::runtime_error(
        "get_cp_insertion_list is not supported in this UHD version");
#endif
}

// CP Removal List
void rfnoc_fft_impl::set_cp_removal_list(const std::vector<uint32_t> cp_lengths)
{
#if UHD_VERSION >= 4080000
    d_fft_ref->set_cp_removal_list(cp_lengths);
#else
    throw std::runtime_error("set_cp_removal_list is not supported in this UHD version");
#endif
}

std::vector<uint32_t> rfnoc_fft_impl::get_cp_removal_list() const
{
#if UHD_VERSION >= 4080000
    return d_fft_ref->get_cp_removal_list();
#else
    throw std::runtime_error("get_cp_removal_list is not supported in this UHD version");
#endif
}
} // namespace uhd
} // namespace gr
