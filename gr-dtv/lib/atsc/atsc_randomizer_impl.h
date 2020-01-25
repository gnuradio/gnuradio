/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_ATSC_RANDOMIZER_IMPL_H
#define INCLUDED_DTV_ATSC_RANDOMIZER_IMPL_H

#include "atsc_randomize.h"
#include "atsc_types.h"
#include <gnuradio/dtv/atsc_randomizer.h>

namespace gr {
namespace dtv {

class atsc_randomizer_impl : public atsc_randomizer
{
private:
    atsc_randomize d_rand;
    int d_segno;
    bool d_field2;

    void reset(void);

public:
    atsc_randomizer_impl();
    ~atsc_randomizer_impl();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_ATSC_RANDOMIZER_IMPL_H */
