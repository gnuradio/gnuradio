/* -*- c++ -*- */
/*
 * Copyright 2015,2016 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_DVBT_INNER_CODER_IMPL_H
#define INCLUDED_DTV_DVBT_INNER_CODER_IMPL_H

#include "dvbt_configure.h"
#include <gnuradio/dtv/dvbt_inner_coder.h>

namespace gr {
namespace dtv {

class dvbt_inner_coder_impl : public dvbt_inner_coder
{
private:
    const dvbt_configure config;

    static const int d_lookup_171[128];
    static const int d_lookup_133[128];

    int d_ninput;
    int d_noutput;

    int d_reg = 0;

    // Code rate k/n
    const int d_k;
    const int d_n;
    // Constellation with m
    const int d_m;

    // input block size in bytes
    const int d_in_bs;
    // bit input buffer
    std::vector<unsigned char> d_in_buff;

    // output block size in bytes
    const int d_out_bs;
    // bit output buffer
    std::vector<unsigned char> d_out_buff;

    inline void generate_codeword(unsigned char in, int& x, int& y);
    inline void generate_punctured_code(dvb_code_rate_t coderate,
                                        unsigned char* in,
                                        unsigned char* out);

public:
    dvbt_inner_coder_impl(int ninput,
                          int noutput,
                          dvb_constellation_t constellation,
                          dvbt_hierarchy_t hierarchy,
                          dvb_code_rate_t coderate);
    ~dvbt_inner_coder_impl() override;
    void forecast(int noutput_items, gr_vector_int& ninput_items_required) override;

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT_INNER_CODER_IMPL_H */
