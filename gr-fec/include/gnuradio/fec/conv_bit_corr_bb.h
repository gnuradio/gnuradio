/* -*- c++ -*- */
/*
 * Copyright 2013-2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FEC_CONV_BIT_CORR_BB_H
#define INCLUDED_FEC_CONV_BIT_CORR_BB_H

#include <gnuradio/block.h>
#include <gnuradio/fec/api.h>
#include <vector>

namespace gr {
namespace fec {

/*!
 * \brief Correlate block in FECAPI
 * \ingroup error_coding_blk
 *
 * \details
 *
 * What does this block do?
 */
class FEC_API conv_bit_corr_bb : virtual public block
{
public:
    // gr::fec::conv_bit_corr_bb::sptr
    typedef boost::shared_ptr<conv_bit_corr_bb> sptr;

    static sptr make(std::vector<unsigned long long> correlator,
                     int corr_sym,
                     int corr_len,
                     int cut,
                     int flush,
                     float thresh);

    /*!
     * This subroutine will find the encoded data garble rate
     * corresponding to a syndrome density of `target', that is created
     * with an annihilating polynomial with 'taps' number of taps.
     */
    virtual float data_garble_rate(int taps, float syn_density) = 0;
};

} /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_CONV_BIT_CORR_BB_H */
