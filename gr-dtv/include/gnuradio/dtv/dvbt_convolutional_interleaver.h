/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_DVBT_CONVOLUTIONAL_INTERLEAVER_H
#define INCLUDED_DTV_DVBT_CONVOLUTIONAL_INTERLEAVER_H

#include <gnuradio/dtv/api.h>
#include <gnuradio/sync_interpolator.h>

namespace gr {
namespace dtv {

/*!
 * \brief Convolutional interleaver.
 * \ingroup dtv
 *
 * ETSI EN 300 744 Clause 4.3.1 \n
 * Forney (Ramsey type III) convolutional interleaver. \n
 * Input: Blocks of I bytes size. \n
 * Output: Stream of 1 byte elements.
 */
class DTV_API dvbt_convolutional_interleaver : virtual public sync_interpolator
{
public:
    typedef boost::shared_ptr<dvbt_convolutional_interleaver> sptr;

    /*!
     * \brief Create a DVB-T convolutional interleaver.
     *
     * \param nsize number of blocks to process. \n
     * \param I size of a block. \n
     * \param M depth length for each element in shift registers.
     */
    static sptr make(int nsize, int I, int M);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT_CONVOLUTIONAL_INTERLEAVER_H */
