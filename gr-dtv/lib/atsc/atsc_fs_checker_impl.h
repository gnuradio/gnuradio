/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_ATSC_FS_CHECKER_IMPL_H
#define INCLUDED_DTV_ATSC_FS_CHECKER_IMPL_H

#include "atsc_syminfo_impl.h"
#include <gnuradio/dtv/atsc_fs_checker.h>

namespace gr {
namespace dtv {

class atsc_fs_checker_impl : public atsc_fs_checker
{
private:
    static constexpr int SRSIZE = 1024; // must be power of two
    int d_index;                        // points at oldest sample
    float d_sample_sr[SRSIZE];          // sample shift register
    atsc::syminfo d_tag_sr[SRSIZE];     // tag shift register
    unsigned char d_bit_sr[SRSIZE];     // binary decision shift register
    int d_field_num;
    int d_segment_num;

    static constexpr int OFFSET_511 = 4;      // offset to second PN 63 pattern
    static constexpr int LENGTH_511 = 511;    // length of PN 63 pattern
    static constexpr int OFFSET_2ND_63 = 578; // offset to second PN 63 pattern
    static constexpr int LENGTH_2ND_63 = 63;  // length of PN 63 pattern

    inline static int wrap(int index) { return index & (SRSIZE - 1); }
    inline static int incr(int index) { return wrap(index + 1); }
    inline static int decr(int index) { return wrap(index - 1); }

public:
    atsc_fs_checker_impl();
    ~atsc_fs_checker_impl() override;

    void reset();

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;
};

} /* namespace dtv */
} /* namespace gr */

#endif /* INCLUDED_DTV_ATSC_FS_CHECKER_IMPL_H */
