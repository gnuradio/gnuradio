/* -*- c++ -*- */
/*
 * Copyright 2019 Ettus Research, a National Instruments Brand.
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_GR_UHD_RFNOC_DUC_IMPL_H
#define INCLUDED_GR_UHD_RFNOC_DUC_IMPL_H

#include <gnuradio/uhd/rfnoc_duc.h>
#include <uhd/rfnoc/duc_block_control.hpp>

namespace gr {
namespace uhd {

class rfnoc_duc_impl : public rfnoc_duc
{
public:
    rfnoc_duc_impl(::uhd::rfnoc::noc_block_base::sptr block_ref);
    ~rfnoc_duc_impl() override;

    /*** API *****************************************************************/
    double set_freq(const double freq,
                    const size_t chan,
                    const ::uhd::time_spec_t time = ::uhd::time_spec_t::ASAP) override;
    double set_input_rate(const double rate, const size_t chan) override;

private:
    ::uhd::rfnoc::duc_block_control::sptr d_duc_ref;
};

} // namespace uhd
} // namespace gr

#endif /* INCLUDED_GR_UHD_RFNOC_DUC_IMPL_H */
