/* -*- c++ -*- */
/*
 * Copyright 2006,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_DIFF_PHASOR_CC_H
#define INCLUDED_GR_DIFF_PHASOR_CC_H

#include <gnuradio/digital/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace digital {

/*!
 * \brief Differential decoding based on phase change.
 * \ingroup symbol_coding_blk
 *
 * \details
 * Uses the phase difference between two symbols to determine the
 * output symbol:
 *
 *     out[i] = in[i] * conj(in[i-1]);
 */
class DIGITAL_API diff_phasor_cc : virtual public sync_block
{
public:
    // gr::digital::diff_phasor_cc::sptr
    typedef boost::shared_ptr<diff_phasor_cc> sptr;

    /*!
     * Make a differential phasor decoding block.
     */
    static sptr make();
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_GR_DIFF_PHASOR_CC_H */
