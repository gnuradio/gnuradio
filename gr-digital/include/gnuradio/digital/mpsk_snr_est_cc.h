/* -*- c++ -*- */
/*
 * Copyright 2011,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_MPSK_SNR_EST_CC_H
#define INCLUDED_DIGITAL_MPSK_SNR_EST_CC_H

#include <gnuradio/digital/api.h>
#include <gnuradio/digital/mpsk_snr_est.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace digital {

/*!
 * \brief A block for computing SNR of a signal.
 * \ingroup measurement_tools_blk
 *
 * \details
 * This block can be used to monitor and retrieve estimations of
 * the signal SNR. It is designed to work in a flowgraph and
 * passes all incoming data along to its output.
 *
 * The block is designed for use with M-PSK signals
 * especially. The type of estimator is specified as the \p type
 * parameter in the constructor. The estimators tend to trade off
 * performance for accuracy, although experimentation should be
 * done to figure out the right approach for a given
 * implementation. Further, the current set of estimators are
 * designed and proven theoretically under AWGN conditions; some
 * amount of error should be assumed and/or estimated for real
 * channel conditions.
 *
 * The estimator is normally placed before clock recovery.
 */
class DIGITAL_API mpsk_snr_est_cc : virtual public sync_block
{
public:
    // gr::digital::mpsk_snr_est_cc::sptr
    typedef std::shared_ptr<mpsk_snr_est_cc> sptr;

    /*! Factory function returning shared pointer of this class
     *
     *  \param type: the type of estimator to use gr::digital::snr_est_type_t
     *  "snr_est_type_t" for details about the available types
     *  \param tag_nsamples: after this many samples, a tag containing
     *  the SNR (key='snr') will be sent
     *  \param alpha: the update rate of internal running average
     *  calculations. Needs to be between 0 and 1, where higher value
     *  adjusts faster to new data.
     */
    static sptr make(snr_est_type_t type, int tag_nsamples = 10000, double alpha = 0.001);

    //! Return the estimated signal-to-noise ratio in decibels
    virtual double snr() = 0;

    //! Return the type of estimator in use
    virtual snr_est_type_t type() const = 0;

    //! Return how many samples between SNR tags
    virtual int tag_nsample() const = 0;

    //! Get the running-average coefficient
    virtual double alpha() const = 0;

    //! Set type of estimator to use
    virtual void set_type(snr_est_type_t t) = 0;

    //! Set the number of samples between SNR tags
    virtual void set_tag_nsample(int n) = 0;

    //! Set the running-average coefficient
    virtual void set_alpha(double alpha) = 0;
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_MPSK_SNR_EST_CC_H */
