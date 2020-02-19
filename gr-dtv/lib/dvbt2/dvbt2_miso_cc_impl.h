/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_DVBT2_MISO_CC_IMPL_H
#define INCLUDED_DTV_DVBT2_MISO_CC_IMPL_H

#include "../dvb/dvb_defines.h"

#include <gnuradio/dtv/dvbt2_miso_cc.h>

namespace gr {
namespace dtv {

class dvbt2_miso_cc_impl : public dvbt2_miso_cc
{
private:
    int miso_items;
    int N_P2;
    int C_P2;
    int N_FC;
    int C_FC;
    int C_DATA;

public:
    dvbt2_miso_cc_impl(dvbt2_extended_carrier_t carriermode,
                       dvbt2_fftsize_t fftsize,
                       dvbt2_pilotpattern_t pilotpattern,
                       dvb_guardinterval_t guardinterval,
                       int numdatasyms,
                       dvbt2_papr_t paprmode);
    ~dvbt2_miso_cc_impl();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT2_MISO_CC_IMPL_H */
