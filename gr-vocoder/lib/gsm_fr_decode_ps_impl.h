/* -*- c++ -*- */
/*
 * Copyright 2005,2011,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_VOCODER_GSM_FR_DECODE_PS_IMPL_H
#define INCLUDED_VOCODER_GSM_FR_DECODE_PS_IMPL_H

#include <gnuradio/vocoder/gsm_fr_decode_ps.h>

extern "C" {
#include "gsm.h"
}

namespace gr {
namespace vocoder {

class gsm_fr_decode_ps_impl : public gsm_fr_decode_ps
{
private:
    struct gsm_state* d_gsm;

public:
    gsm_fr_decode_ps_impl();
    ~gsm_fr_decode_ps_impl() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace vocoder */
} /* namespace gr */

#endif /* INCLUDED_VOCODER_GSM_FR_DECODE_PS_IMPL_H */
