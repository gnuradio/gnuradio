/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_ATSC_DERANDOMIZER_IMPL_H
#define INCLUDED_DTV_ATSC_DERANDOMIZER_IMPL_H

#include "atsc_randomize.h"
#include <gnuradio/dtv/atsc_derandomizer.h>

namespace gr {
namespace dtv {

class atsc_derandomizer_impl : public atsc_derandomizer
{
private:
    atsc_randomize d_rand;

public:
    atsc_derandomizer_impl();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace dtv */
} /* namespace gr */

#endif /* INCLUDED_DTV_ATSC_DERANDOMIZER_IMPL_H */
