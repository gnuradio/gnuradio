/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_CONSTELLATION_DECODER_CB_IMPL_H
#define INCLUDED_DIGITAL_CONSTELLATION_DECODER_CB_IMPL_H

#include <gnuradio/digital/constellation_encoder_bc.h>

namespace gr {
namespace digital {

class constellation_encoder_bc_impl : public constellation_encoder_bc
{
private:
    constellation_sptr d_constellation;

public:
    constellation_encoder_bc_impl(constellation_sptr constellation);
    ~constellation_encoder_bc_impl() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_CONSTELLATION_DECODER_CB_IMPL_H */
