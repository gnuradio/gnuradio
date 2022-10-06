/* -*- c++ -*- */
/*
 * Copyright 2019 Ettus Research, a National Instruments Brand.
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_GR_UHD_RFNOC_TX_RADIO_IMPL_H
#define INCLUDED_GR_UHD_RFNOC_TX_RADIO_IMPL_H

#include <gnuradio/uhd/rfnoc_tx_radio.h>
#include <uhd/rfnoc/radio_control.hpp>

namespace gr {
namespace uhd {

class rfnoc_tx_radio_impl : public rfnoc_tx_radio
{
public:
    rfnoc_tx_radio_impl(::uhd::rfnoc::noc_block_base::sptr block_ref);
    ~rfnoc_tx_radio_impl() override;

    /*** API *****************************************************************/
    double set_rate(const double rate) override;
    void set_antenna(const std::string& antenna, const size_t chan) override;
    double set_frequency(const double frequency, const size_t chan) override;
    void set_tune_args(const ::uhd::device_addr_t& args, const size_t chan) override;
    double set_gain(const double gain, const size_t chan) override;
    double
    set_gain(const double gain, const std::string& name, const size_t chan) override;
    void set_gain_profile(const std::string& profile, const size_t chan) override;
    double set_bandwidth(const double bandwidth, const size_t chan) override;
    void set_lo_source(const std::string& source,
                       const std::string& name,
                       const size_t chan) override;
    void set_lo_export_enabled(const bool enabled,
                               const std::string& name,
                               const size_t chan) override;
    double
    set_lo_freq(const double freq, const std::string& name, const size_t chan) override;
    void set_dc_offset(const std::complex<double>& offset, const size_t chan) override;
    void set_iq_balance(const std::complex<double>& correction,
                        const size_t chan) override;

private:
    ::uhd::rfnoc::radio_control::sptr d_wrapped_ref;
};

} // namespace uhd
} // namespace gr

#endif /* INCLUDED_GR_UHD_RFNOC_TX_RADIO_IMPL_H */
