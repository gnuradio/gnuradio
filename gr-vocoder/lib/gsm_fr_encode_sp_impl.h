/* -*- c++ -*- */
/*
 * Copyright 2005,2011,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_VOCODER_GSM_FR_ENCODE_SP_IMPL_H
#define INCLUDED_VOCODER_GSM_FR_ENCODE_SP_IMPL_H

#include <gnuradio/vocoder/gsm_fr_encode_sp.h>

extern "C" {
#include "gsm.h"
}

namespace gr {
namespace vocoder {

class gsm_fr_encode_sp_impl : public gsm_fr_encode_sp
{
private:
    struct gsm_state* d_gsm;

public:
    gsm_fr_encode_sp_impl();
    ~gsm_fr_encode_sp_impl() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace vocoder */
} /* namespace gr */

#endif /* INCLUDED_VOCODER_GSM_FR_ENCODE_SP_IMPL_H */
