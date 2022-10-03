/* -*- c++ -*- */
/*
 * Copyright 2022 Ettus Research, A National Instruments Brand.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "rfnoc_fir_filter_impl.h"
#include <gnuradio/io_signature.h>
#include <uhd/version.hpp>
#include <volk/volk.h>

namespace gr {
namespace uhd {

rfnoc_fir_filter::sptr rfnoc_fir_filter::make(rfnoc_graph::sptr graph,
                                              const ::uhd::device_addr_t& block_args,
                                              const int device_select,
                                              const int instance)
{
    return gnuradio::make_block_sptr<rfnoc_fir_filter_impl>(
        rfnoc_block::make_block_ref(graph, block_args, "FIR", device_select, instance));
}


rfnoc_fir_filter_impl::rfnoc_fir_filter_impl(::uhd::rfnoc::noc_block_base::sptr block_ref)
    : rfnoc_block(block_ref),
      d_fir_filter_ref(get_block_ref<::uhd::rfnoc::fir_filter_block_control>())
{
}

rfnoc_fir_filter_impl::~rfnoc_fir_filter_impl() {}

/******************************************************************************
 * rfnoc_fir_filter API
 *****************************************************************************/
void rfnoc_fir_filter_impl::set_coefficients(const std::vector<float>& coeffs,
                                             const size_t chan)
{
    // UHD allows passing empty vectors, so we keep that behaviour
    if (coeffs.empty()) {
        set_coefficients(std::vector<int16_t>{}, chan);
    }
    std::vector<int16_t> coeffs_int16(coeffs.size());

    const float* in = coeffs.data();
    int16_t* out = coeffs_int16.data();

    volk_32f_s32f_convert_16i(out, in, 32768.0, coeffs.size());
    set_coefficients(coeffs_int16, chan);
}

void rfnoc_fir_filter_impl::set_coefficients(const std::vector<int16_t>& coeffs,
                                             const size_t chan)
{
    _check_chan_valid(chan);
#if UHD_VERSION >= 4020000
    d_fir_filter_ref->set_coefficients(coeffs, chan);
#else
    d_fir_filter_ref->set_coefficients(coeffs);
#endif
}

size_t rfnoc_fir_filter_impl::get_max_num_coefficients(const size_t chan)
{
    _check_chan_valid(chan);
#if UHD_VERSION >= 4020000
    return d_fir_filter_ref->get_max_num_coefficients(chan);
#else
    return d_fir_filter_ref->get_max_num_coefficients();
#endif
}

std::vector<int16_t> rfnoc_fir_filter_impl::get_coefficients(const size_t chan)
{
    _check_chan_valid(chan);
#if UHD_VERSION >= 4020000
    return d_fir_filter_ref->get_coefficients(chan);
#else
    return d_fir_filter_ref->get_coefficients();
#endif
}


/******************************************************************************
 * Helpers
 *****************************************************************************/
void rfnoc_fir_filter_impl::_check_chan_valid(const size_t chan)
{
#if UHD_VERSION >= 4020000
    return;
#else
    if (chan > 0) {
        d_logger->warn("Invalid channel index: {}. This version of UHD only "
                       "supports one channel.",
                       chan);
    }
#endif
}

} // namespace uhd
} /* namespace gr */
