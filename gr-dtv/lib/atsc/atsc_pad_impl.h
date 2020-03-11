/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_ATSC_PAD_IMPL_H
#define INCLUDED_DTV_ATSC_PAD_IMPL_H

#include "atsc_types.h"
#include <gnuradio/dtv/atsc_pad.h>

namespace gr {
namespace dtv {

class atsc_pad_impl : public atsc_pad
{
public:
    atsc_pad_impl();
    ~atsc_pad_impl();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_ATSC_PAD_IMPL_H */
