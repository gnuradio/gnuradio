/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_ATSC_INTERLEAVER_IMPL_H
#define INCLUDED_DTV_ATSC_INTERLEAVER_IMPL_H

#include "atsc_types.h"
#include <gnuradio/dtv/atsc_interleaver.h>

namespace gr {
namespace dtv {

class atsc_interleaver_impl : public atsc_interleaver
{
private:
    int I, J;
    int commutator;
    unsigned char* registers;
    int* pointers;

public:
    atsc_interleaver_impl();
    ~atsc_interleaver_impl();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_ATSC_INTERLEAVER_IMPL_H */
