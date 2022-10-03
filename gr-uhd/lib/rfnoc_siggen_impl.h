/* -*- c++ -*- */
/*
 * Copyright 2022 Ettus Research, A National Instruments Brand.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_GR_UHD_RFNOC_SIGGEN_IMPL_H
#define INCLUDED_GR_UHD_RFNOC_SIGGEN_IMPL_H

#include <gnuradio/uhd/rfnoc_siggen.h>
#include <uhd/rfnoc/siggen_block_control.hpp>

namespace gr {
namespace uhd {

class rfnoc_siggen_impl : public rfnoc_siggen
{
public:
    rfnoc_siggen_impl(::uhd::rfnoc::noc_block_base::sptr block_ref);
    ~rfnoc_siggen_impl() override;

    /*** API *****************************************************************/
    void set_amplitude(const double amplitude, const size_t chan) override;
    void set_constant(const std::complex<double> constant, const size_t chan) override;
    void set_enable(const bool enable, const size_t chan) override;
    void set_sine_phase_increment(const double phase_inc, const size_t chan) override;
    void set_sine_frequency(const double frequency,
                            const double sample_rate,
                            const size_t chan) override;
    void set_waveform(const siggen_waveform type, const size_t chan) override;
    void set_waveform(const std::string& type, const size_t chan) override;
    void set_samples_per_packet(const size_t spp, const size_t chan) override;

    double get_amplitude(const size_t chan) override;
    std::complex<double> get_constant(const size_t chan) override;
    bool get_enable(const size_t chan) override;
    double get_sine_phase_increment(const size_t chan) override;
    siggen_waveform get_waveform(const size_t chan) override;
    std::string get_waveform_string(const size_t chan) override;
    size_t get_samples_per_packet(const size_t chan) override;

private:
    ::uhd::rfnoc::siggen_block_control::sptr d_siggen_ref;
};

} // namespace uhd
} // namespace gr

#endif /* INCLUDED_GR_UHD_RFNOC_SIGGEN_IMPL_H */
