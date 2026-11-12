/* -*- c++ -*- */
/*
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_CHANNELS_AMP_BAL_H
#define INCLUDED_CHANNELS_AMP_BAL_H

#include <gnuradio/channels/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace channels {

/*!
 * \brief IQ amplitude balance
 * \ingroup channel_models_blk
 *
 * \details
 * Scales Q by rms(I) / rms(Q), with the RMS values tracked by single-pole
 * averages of I^2 and Q^2.
 */
class CHANNELS_API amp_bal : virtual public sync_block
{
public:
    typedef std::shared_ptr<amp_bal> sptr;

    /*!
     * \param alpha RMS averaging coefficient, in (0, 1]
     */
    static sptr make(double alpha = 1e-4);

    virtual double alpha() const = 0;
    virtual void set_alpha(double alpha) = 0;
};

} /* namespace channels */
} /* namespace gr */

#endif /* INCLUDED_CHANNELS_AMP_BAL_H */
