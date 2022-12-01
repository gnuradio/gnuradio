/* -*- c++ -*- */
/*
 * Copyright 2019 Ettus Research, a National Instruments Brand.
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_GR_UHD_RFNOC_WINDOW_IMPL_H
#define INCLUDED_GR_UHD_RFNOC_WINDOW_IMPL_H

#include <gnuradio/uhd/rfnoc_window.h>
#include <uhd/rfnoc/window_block_control.hpp>

namespace gr {
namespace uhd {

class rfnoc_window_impl : public rfnoc_window
{
public:
    rfnoc_window_impl(::uhd::rfnoc::noc_block_base::sptr block_ref);
    ~rfnoc_window_impl() override;

    /*** API *****************************************************************/
    void set_coefficients(const std::vector<float>& coeffs, const size_t chan) override;
    void set_coefficients(const std::vector<int16_t>& coeffs, const size_t chan) override;
    size_t get_max_num_coefficients(const size_t chan) override;
    std::vector<int16_t> get_coefficients(const size_t chan) override;

private:
    ::uhd::rfnoc::window_block_control::sptr d_window_ref;
};

} // namespace uhd
} // namespace gr

#endif /* INCLUDED_GR_UHD_RFNOC_WINDOW_IMPL_H */
