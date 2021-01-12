/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_MSK_TIMING_RECOVERY_CC_H
#define INCLUDED_DIGITAL_MSK_TIMING_RECOVERY_CC_H

#include <gnuradio/block.h>
#include <gnuradio/digital/api.h>

namespace gr {
namespace digital {

/*!
 * \brief MSK/GMSK timing recovery
 * \ingroup synchronizers_blk
 * \ingroup deprecated_blk
 *
 * This block performs timing synchronization on CPM modulations using a
 * fourth-order nonlinearity feedback method which is non-data-aided. The
 * block does not require prior phase synchronization but is relatively
 * sensitive to frequency offset (keep offset to 0.1x symbol rate).
 *
 * For details on the algorithm, see:
 * A.N. D'Andrea, U. Mengali, R. Reggiannini: A digital approach to clock
 * recovery in generalized minimum shift keying. IEEE Transactions on
 * Vehicular Technology, Vol. 39, Issue 3.
 */
class DIGITAL_API msk_timing_recovery_cc : virtual public gr::block
{
public:
    typedef std::shared_ptr<msk_timing_recovery_cc> sptr;

    /*!
     * \brief Make an MSK timing recovery block.
     *
     * \param sps: Samples per symbol
     * \param gain: Loop gain of timing error filter (try 0.05)
     * \param limit: Relative limit of timing error (try 0.1 for 10% error max)
     * \param osps: Output samples per symbol
     *
     */
    static sptr make(float sps, float gain, float limit, int osps);

    virtual void set_gain(float gain) = 0;
    virtual float get_gain(void) = 0;

    virtual void set_limit(float limit) = 0;
    virtual float get_limit(void) = 0;

    virtual void set_sps(float sps) = 0;
    virtual float get_sps(void) = 0;
};

} // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_MSK_TIMING_RECOVERY_CC_H */
