/* -*- c++ -*- */
/*
 * Copyright 2008,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_WAVELET_WVPS_FF_H
#define INCLUDED_WAVELET_WVPS_FF_H

#include <gnuradio/sync_decimator.h>
#include <gnuradio/wavelet/api.h>

class wavelet_wvps_ff;

namespace gr {
namespace wavelet {

/*!
 * \brief computes the Wavelet Power Spectrum from a set of wavelet coefficients
 * \ingroup wavelet_blk
 */
class WAVELET_API wvps_ff : virtual public sync_block
{
public:
    // gr::wavelet::wvps_ff::sptr
    typedef boost::shared_ptr<wvps_ff> sptr;

    /*!
      \param ilen
     */
    static sptr make(int ilen);
};

} /* namespace wavelet */
} /* namespace gr */

#endif /* INCLUDED_WAVELET_WVPS_FF_H */
