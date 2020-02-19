/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_DVBT_REED_SOLOMON_ENC_H
#define INCLUDED_DTV_DVBT_REED_SOLOMON_ENC_H

#include <gnuradio/block.h>
#include <gnuradio/dtv/api.h>

namespace gr {
namespace dtv {

/*!
 * \brief Reed Solomon encoder
 * \ingroup dtv
 *
 * ETSI EN 300 744 Clause 4.3.2 \n
 * RS(N=204,K=239,T=8).
 */
class DTV_API dvbt_reed_solomon_enc : virtual public block
{
public:
    typedef boost::shared_ptr<dvbt_reed_solomon_enc> sptr;

    /*!
     * \brief Create a Reed Solomon encoder.
     *
     * \param p characteristic of GF(p^m).
     * \param m we use GF(p^m).
     * \param gfpoly Generator Polynomial.
     * \param n length of codeword of RS coder.
     * \param k length of information sequence of RS encoder.
     * \param t number of corrected errors.
     * \param s shortened length.
     * \param blocks number of blocks to process at once.
     */
    static sptr make(int p, int m, int gfpoly, int n, int k, int t, int s, int blocks);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT_REED_SOLOMON_ENC_H */
