/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_OFDM_SYNC_SC_CFB_IMPL_H
#define INCLUDED_DIGITAL_OFDM_SYNC_SC_CFB_IMPL_H

#include <gnuradio/blocks/plateau_detector_fb.h>
#include <gnuradio/digital/ofdm_sync_sc_cfb.h>

namespace gr {
namespace digital {

class ofdm_sync_sc_cfb_impl : public ofdm_sync_sc_cfb
{
public:
    ofdm_sync_sc_cfb_impl(int fft_len,
                          int cp_len,
                          bool use_even_carriers,
                          float threshold);
    ~ofdm_sync_sc_cfb_impl();

    virtual void set_threshold(float threshold);
    virtual float threshold() const;

private:
    gr::blocks::plateau_detector_fb::sptr d_plateau_detector;
};

} // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_OFDM_SYNC_SC_CFB_IMPL_H */
