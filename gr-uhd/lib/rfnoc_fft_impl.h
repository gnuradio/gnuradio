/* -*- c++ -*- */
/*
 * Copyright 2022 Ettus Research, A National Instruments Brand.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_GR_UHD_RFNOC_FFT_IMPL_H
#define INCLUDED_GR_UHD_RFNOC_FFT_IMPL_H

#include <gnuradio/uhd/rfnoc_fft.h>
#include <uhd/rfnoc/fft_block_control.hpp>

namespace gr {
namespace uhd {

class rfnoc_fft_impl : public rfnoc_fft
{
public:
    rfnoc_fft_impl(::uhd::rfnoc::noc_block_base::sptr block_ref);
    ~rfnoc_fft_impl() override;

    /*** API *****************************************************************/
    void set_direction(const fft_direction direction) override;
    void set_direction(const std::string& direction) override;
    std::string get_direction_string() const override;
    fft_direction get_direction() const override;

    void set_magnitude(const fft_magnitude magnitude) override;
    void set_magnitude(const std::string& magnitude) override;
    std::string get_magnitude_string() const override;
    fft_magnitude get_magnitude() const override;

    void set_shift_config(const fft_shift shift) override;
    void set_shift_config(const std::string& shift) override;
    std::string get_shift_config_string() const override;
    fft_shift get_shift_config() const override;

    void set_scaling_factor(const double scaling_factor) override;

    void set_scaling(const uint32_t scaling) override;
    uint32_t get_scaling() const override;

    void set_length(const uint32_t length) override;
    uint32_t get_length() const override;

    void set_bypass_mode(const bool bypass) override;
    bool get_bypass_mode() const override;

    uint32_t get_nipc() const override;

    uint32_t get_max_length() const override;
    uint32_t get_max_cp_length() const override;
    uint32_t get_max_cp_insertion_list_length() const override;
    uint32_t get_max_cp_removal_list_length() const override;

    void set_cp_insertion_list(const std::vector<uint32_t> cp_lengths) override;
    std::vector<uint32_t> get_cp_insertion_list() const override;

    void set_cp_removal_list(const std::vector<uint32_t> cp_lengths) override;
    std::vector<uint32_t> get_cp_removal_list() const override;

private:
    ::uhd::rfnoc::fft_block_control::sptr d_fft_ref;
};

} // namespace uhd
} // namespace gr

#endif /* INCLUDED_GR_UHD_RFNOC_FFT_IMPL_H */
