/* -*- c++ -*- */
/*
 * Copyright 2006,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_DIFF_DECODER_BB_H
#define INCLUDED_GR_DIFF_DECODER_BB_H

#include <gnuradio/digital/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace digital {

/*!
 * \brief Differential encoder: y[0] = (x[0] - x[-1]) % M
 * \ingroup symbol_coding_blk
 *
 * \details
 * Uses current and previous symbols and the alphabet modulus to
 * perform differential decoding.
 */
class DIGITAL_API diff_decoder_bb : virtual public sync_block
{
public:
    // gr::digital::diff_decoder_bb::sptr
    typedef boost::shared_ptr<diff_decoder_bb> sptr;

    /*!
     * Make a differential decoder block.
     *
     * \param modulus Modulus of code's alphabet
     */
    static sptr make(unsigned int modulus);
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_GR_DIFF_DECODER_BB_H */
