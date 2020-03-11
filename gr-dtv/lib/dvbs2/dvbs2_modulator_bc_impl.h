/* -*- c++ -*- */
/*
 * Copyright 2015,2016 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_DVBS2_MODULATOR_BC_IMPL_H
#define INCLUDED_DTV_DVBS2_MODULATOR_BC_IMPL_H

#include "../dvb/dvb_defines.h"

#include <gnuradio/dtv/dvbs2_modulator_bc.h>

namespace gr {
namespace dtv {

class dvbs2_modulator_bc_impl : public dvbs2_modulator_bc
{
private:
    int signal_constellation;
    int signal_interpolation;
    gr_complex m_bpsk[2][2];
    gr_complex m_qpsk[4];
    gr_complex m_8psk[8];
    gr_complex m_16apsk[16];
    gr_complex m_32apsk[32];
    gr_complex m_64apsk[64];
    gr_complex m_128apsk[128];
    gr_complex m_256apsk[256];

public:
    dvbs2_modulator_bc_impl(dvb_framesize_t framesize,
                            dvb_code_rate_t rate,
                            dvb_constellation_t constellation,
                            dvbs2_interpolation_t interpolation);
    ~dvbs2_modulator_bc_impl();

    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DVBS2_MODULATOR_BC_IMPL_H */
