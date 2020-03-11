/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_ATSC_DEPAD_IMPL_H
#define INCLUDED_DTV_ATSC_DEPAD_IMPL_H

#include "atsc_types.h"
#include <gnuradio/dtv/atsc_depad.h>

namespace gr {
namespace dtv {

class atsc_depad_impl : public atsc_depad
{
public:
    atsc_depad_impl();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace dtv */
} /* namespace gr */

#endif /* INCLUDED_DTV_ATSC_DEPAD_IMPL_H */
