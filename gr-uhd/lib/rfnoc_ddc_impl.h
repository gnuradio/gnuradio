/* -*- c++ -*- */
/*
 * Copyright 2019 Ettus Research, a National Instruments Brand.
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_GR_UHD_RFNOC_DDC_IMPL_H
#define INCLUDED_GR_UHD_RFNOC_DDC_IMPL_H

#include <gnuradio/uhd/rfnoc_ddc.h>
#include <uhd/rfnoc/ddc_block_control.hpp>

namespace gr {
namespace uhd {

class rfnoc_ddc_impl : public rfnoc_ddc
{
public:
    rfnoc_ddc_impl(::uhd::rfnoc::noc_block_base::sptr block_ref);
    ~rfnoc_ddc_impl() override;

    /*** API *****************************************************************/
    double set_freq(const double freq,
                    const size_t chan,
                    const ::uhd::time_spec_t time = ::uhd::time_spec_t::ASAP) override;
    double set_output_rate(const double rate, const size_t chan) override;

private:
    ::uhd::rfnoc::ddc_block_control::sptr d_ddc_ref;
};

} // namespace uhd
} // namespace gr

#endif /* INCLUDED_GR_UHD_RFNOC_DDC_IMPL_H */
