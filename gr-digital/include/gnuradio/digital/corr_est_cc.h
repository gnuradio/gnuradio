/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_CORR_EST_CC_CC_H
#define INCLUDED_DIGITAL_CORR_EST_CC_CC_H

#include <gnuradio/digital/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace digital {

/*!
 * \brief Correlate stream with a pre-defined sequence and estimate peak
 * \ingroup synchronizers_blk
 *
 * \details
 * Input:
 * \li Stream of complex samples.
 *
 * Output:
 * \li Output stream that just passes the input complex samples
 * \li tag 'phase_est': estimate of phase offset
 * \li tag 'time_est': estimate of symbol timing offset
 * \li tag 'corr_est': the correlation value of the estimates
 * \li tag 'amp_est': 1 over the estimated amplitude
 * \li tag 'corr_start': the start sample of the correlation and the value
 *
 * \li Optional 2nd output stream providing the advanced correlator output
 *
 * This block is designed to search for a sync word by correlation
 * and uses the results of the correlation to get a time and phase
 * offset estimate. These estimates are passed downstream as
 * stream tags for use by follow-on synchronization blocks.
 *
 * The sync word is provided as a set of symbols after being
 * filtered by a baseband matched filter.
 *
 * The phase_est tag can be used by downstream blocks to adjust
 * their phase estimator/correction loops, and is currently
 * implemented by the gr::digital::costas_loop_cc block.
 *
 * The time_est tag can be used to adjust the sampling timing
 * estimate of any downstream synchronization blocks and is
 * currently implemented by the gr::digital::pfb_clock_sync_ccf
 * block.
 *
 * The caller must provide a "time_est" and "phase_est" tag
 * marking delay from the start of the correlated signal segment,
 * in order to mark the proper point in the sync word for
 * downstream synchronization blocks.  Generally this block cannot
 * know where the actual sync word symbols are located relative to
 * "corr_start", given that some modulations have pulses with
 * intentional ISI.  The user should manually examine the primary
 * output and the "corr_start" tag position to determine the
 * required tag delay settings for the particular modulation,
 * sync word, and downstream blocks used.
 *
 * For a discussion of the properties of complex correlations,
 * with respect to signal processing, see:
 * Marple, Jr., S. L., "Estimating Group Delay and Phase Delay
 * via Discrete-Time 'Analytic' Cross-Correlation, _IEEE_Transcations_
 * _on_Signal_Processing_, Volume 47, No. 9, September 1999
 *
 */
typedef enum {
    THRESHOLD_DYNAMIC,
    THRESHOLD_ABSOLUTE,
} tm_type;

class DIGITAL_API corr_est_cc : virtual public sync_block
{
public:
    typedef std::shared_ptr<corr_est_cc> sptr;

    /*!
     * Make a block that correlates against the \p symbols vector
     * and outputs a phase and symbol timing estimate.
     *
     * \param symbols           Set of symbols to correlate against (e.g., a
     *                          sync word).
     * \param sps               Samples per symbol
     * \param mark_delay        tag marking delay in samples after the
     *                          corr_start tag
     * \param threshold         Threshold of correlator.
     *                          The meaning of this parameter depends on the threshold
     *                          method used. For DYNAMIC threshold method, this
     *                          parameter is actually 1 - Probability of False
     *                          Alarm (under some inaccurate assumptions). The
     *                          code performs the check
     *                          |r[k]|^2 + |r[k+1]|^2 <> -log(1-threshold)*2*E,
     *                          where r[k] is the correlated incoming signal,
     *                          and E is the average sample energy of the
     *                          correlated signal. For ABSOLUTE threshold method,
     *                          this parameter sets the threshold to a fraction
     *                          of the maximum squared autocorrelation.  The code
     *                          performs the check |r[k]|^2 <> threshold * R^2,
     *                          where R is the precomputed max autocorrelation
     *                          of the given sync word. Default is 0.9
     * \param threshold_method  Method for computing threshold.
     */
    static sptr make(const std::vector<gr_complex>& symbols,
                     float sps,
                     unsigned int mark_delay,
                     float threshold = 0.9,
                     tm_type threshold_method = THRESHOLD_ABSOLUTE);

    virtual std::vector<gr_complex> symbols() const = 0;
    virtual void set_symbols(const std::vector<gr_complex>& symbols) = 0;

    virtual unsigned int mark_delay() const = 0;
    virtual void set_mark_delay(unsigned int mark_delay) = 0;

    virtual float threshold() const = 0;
    virtual void set_threshold(float threshold) = 0;
};

} // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_CORR_EST_CC_H */
