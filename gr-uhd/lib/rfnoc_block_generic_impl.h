/* -*- c++ -*- */
/*
 * Copyright 2019 Ettus Research, a National Instruments Brand.
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_GR_UHD_RFNOC_BLOCK_GENERIC_IMPL_H
#define INCLUDED_GR_UHD_RFNOC_BLOCK_GENERIC_IMPL_H

#include <gnuradio/uhd/rfnoc_block_generic.h>

namespace gr {
namespace uhd {

class rfnoc_block_generic_impl : public rfnoc_block_generic
{
public:
    rfnoc_block_generic_impl(::uhd::rfnoc::noc_block_base::sptr block_ref);
    ~rfnoc_block_generic_impl() override;
};

} // namespace uhd
} // namespace gr

#endif /* INCLUDED_GR_UHD_RFNOC_BLOCK_GENERIC_IMPL_H */
