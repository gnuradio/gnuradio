/* -*- c++ -*- */
/*
 * Copyright 2006,2011,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_BINARY_SLICER_FB_H
#define INCLUDED_DIGITAL_BINARY_SLICER_FB_H

#include <gnuradio/digital/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace digital {

/*!
 * \brief Slice float binary symbol producing 1 bit output
 * \ingroup symbol_coding_blk
 *
 * \details
 * \li input[0][n] < 0 --> 0
 * \li input[0][n] >= 0 --> 1
 */
class DIGITAL_API binary_slicer_fb : virtual public sync_block
{
public:
    // gr::digital::binary_slicer_fb::sptr
    typedef boost::shared_ptr<binary_slicer_fb> sptr;

    /*!
     * \brief Make binary symbol slicer block.
     */
    static sptr make();
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_BINARY_SLICER_FB_H */
