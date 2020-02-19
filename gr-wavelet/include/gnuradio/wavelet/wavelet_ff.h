/* -*- c++ -*- */
/*
 * Copyright 2008,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_WAVELET_WAVELET_FF_H
#define INCLUDED_WAVELET_WAVELET_FF_H

#include <gnuradio/sync_block.h>
#include <gnuradio/wavelet/api.h>

namespace gr {
namespace wavelet {

/*!
 * \brief Compute wavelet transform using gsl routines.
 * \ingroup wavelet_blk
 */
class WAVELET_API wavelet_ff : virtual public sync_block
{
public:
    // gr::wavelet::wavelet_ff:sptr
    typedef boost::shared_ptr<wavelet_ff> sptr;

    /*!
     * \param size
     * \param order
     * \param forward
     */
    static sptr make(int size = 1024, int order = 20, bool forward = true);
};

} /* namespace wavelet */
} /* namespace gr */

#endif /* INCLUDED_WAVELET_WAVELET_FF_H */
