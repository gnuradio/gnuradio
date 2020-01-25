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
    typedef boost::shared_ptr<hilbert_fc> sptr;

    /*!
     * Build a Hilbert transformer filter block.
     *
     * \param ntaps The number of taps for the filter.
     * \param window Window type (see firdes::win_type) to use.
     * \param beta Beta value for a Kaiser window.
     */
    static sptr make(unsigned int ntaps,
                     firdes::win_type window = firdes::WIN_HAMMING,
                     double beta = 6.76);
};

} /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_FILTER_HILBERT_FC_H */
