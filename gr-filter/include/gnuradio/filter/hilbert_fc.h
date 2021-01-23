/* -*- c++ -*- */
/*
 * Copyright 2004,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FILTER_HILBERT_FC_H
#define INCLUDED_FILTER_HILBERT_FC_H

#include <gnuradio/fft/window.h>
#include <gnuradio/filter/api.h>
#include <gnuradio/filter/firdes.h>
#include <gnuradio/sync_block.h>
#include <gnuradio/types.h>

namespace gr {
namespace filter {

/*!
 * \brief Hilbert transformer.
 * \ingroup filter_blk
 *
 * \details
 * real output is input appropriately delayed.
 * imaginary output is hilbert filtered (90 degree phase shift)
 * version of input.
 */
class FILTER_API hilbert_fc : virtual public sync_block
{
public:
    // gr::filter::hilbert_fc::sptr
    typedef std::shared_ptr<hilbert_fc> sptr;

    /*!
     * Build a Hilbert transformer filter block.
     *
     * \param ntaps The number of taps for the filter.
     * \param window Window type (see fft::window::win_type) to use.
     * \param param Parameter value for a Kaiser/Exp/Gaussian/Tukey window.
     */
    static sptr make(unsigned int ntaps,
                     fft::window::win_type window = fft::window::win_type::WIN_HAMMING,
                     double param = 6.76);
};

} /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_FILTER_HILBERT_FC_H */
