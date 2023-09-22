/* -*- c++ -*- */
/*
 * Copyright 2011,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_PROBE_MPSK_SNR_EST_C_H
#define INCLUDED_DIGITAL_PROBE_MPSK_SNR_EST_C_H

#include <gnuradio/digital/api.h>
#include <gnuradio/digital/mpsk_snr_est.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace digital {

/*!
 * \brief A probe for computing SNR of a PSK signal.
 * \ingroup measurement_tools_blk
 *
 * \details
 * This is a probe block (a sink) that can be used to monitor and
 * retrieve estimations of the signal SNR. This probe is designed
 * for use with M-PSK signals especially. The type of estimator is
 * specified as the \p type parameter in the constructor. The
 * estimators tend to trade off performance for accuracy, although
 * experimentation should be done to figure out the right approach
 * for a given implementation. Further, the current set of
 * estimators are designed and proven theoretically under AWGN
 * conditions; some amount of error should be assumed and/or
 * estimated for real channel conditions.
 *
 * The block has three output message ports that will emit a
 * message every msg_samples number of samples. These message
 * ports are:
 * \li snr: the current SNR estimate (in dB)
 * \li signal: the current signal power estimate (in dBx)
 * \li noise: the current noise power estimate (in dBx)
 *
 * Some calibration is required to convert dBx of the signal and
 * noise power estimates to real measurements, such as dBm.
 *
 * The estimator is normally placed before clock recovery.
 */
class DIGITAL_API probe_mpsk_snr_est_c : virtual public sync_block
{
public:
    // gr::digital::probe_mpsk_snr_est_c::sptr
    typedef std::shared_ptr<probe_mpsk_snr_est_c> sptr;

    /*! Make an MPSK SNR probe.
     *
     *  Parameters:
     *
     *  \param type: the type of estimator to use see
     *   gr::digital::snr_est_type_t for details about the types.
     *  \param msg_nsamples: [not implemented yet] after this many
     *   samples, a message containing the SNR (key='snr') will be sent
     *  \param alpha: the update rate of internal running average
     *   calculations. Needs to be between 0 and 1, where higher value
     *   adjusts faster to new data.
     */
    static sptr make(snr_est_type_t type, int msg_nsamples = 10000, double alpha = 0.001);

    //! Return the estimated signal-to-noise ratio in decibels
    virtual double snr() = 0;

    //! Return the estimated signal power in decibels
    virtual double signal() = 0;

    //! Return the estimated noise power in decibels
    virtual double noise() = 0;

    //! Return the type of estimator in use
    virtual snr_est_type_t type() const = 0;

    //! Return how many samples between SNR messages
    virtual int msg_nsample() const = 0;

    //! Get the running-average coefficient
    virtual double alpha() const = 0;

    //! Set type of estimator to use
    virtual void set_type(snr_est_type_t t) = 0;

    //! Set the number of samples between SNR messages
    virtual void set_msg_nsample(int n) = 0;

    //! Set the running-average coefficient
    virtual void set_alpha(double alpha) = 0;
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_PROBE_MPSK_SNR_EST_C_H */
