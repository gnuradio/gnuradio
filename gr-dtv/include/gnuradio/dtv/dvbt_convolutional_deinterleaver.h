/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_DVBT_CONVOLUTIONAL_DEINTERLEAVER_H
#define INCLUDED_DTV_DVBT_CONVOLUTIONAL_DEINTERLEAVER_H

#include <gnuradio/block.h>
#include <gnuradio/dtv/api.h>

namespace gr {
namespace dtv {

/*!
 * \brief Convolutional deinterleaver.
 * \ingroup dtv
 *
 * ETSI EN 300 744 Clause 4.3.1 \n
 * Forney (Ramsey type III) convolutional deinterleaver. \n
 * Data input: Stream of 1 byte elements. \n
 * Data output: Blocks of I bytes size.
 */
class DTV_API dvbt_convolutional_deinterleaver : virtual public block
{
public:
    typedef boost::shared_ptr<dvbt_convolutional_deinterleaver> sptr;

    /*!
     * \brief Create a DVB-T convolutional deinterleaver.
     *
     * \param nsize number of blocks to process. \n
     * \param I size of a block. \n
     * \param M depth length for each element in shift registers.
     */
    static sptr make(int nsize, int I, int M);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT_CONVOLUTIONAL_DEINTERLEAVER_H */
