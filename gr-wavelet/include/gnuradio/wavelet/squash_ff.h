/* -*- c++ -*- */
/*
 * Copyright 2008,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_WAVELET_SQUASH_FF_H
#define INCLUDED_WAVELET_SQUASH_FF_H

#include <gnuradio/sync_block.h>
#include <gnuradio/wavelet/api.h>

namespace gr {
namespace wavelet {

/*!
 * \brief Implements cheap resampling of spectrum directly from
 * spectral points, using gsl interpolation
 * \ingroup misc
 */
class WAVELET_API squash_ff : virtual public sync_block
{
public:
    // gr::wavelet::squash_ff::sptr
    typedef boost::shared_ptr<squash_ff> sptr;

    /*!
     * \param igrid
     * \param ogrid
     */
    static sptr make(const std::vector<float>& igrid, const std::vector<float>& ogrid);
};

} /* namespace wavelet */
} /* namespace gr */

#endif
