/* -*- c++ -*- */
/*
 * Copyright 2007,2013,2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_PEAK_DETECTOR2_FB_H
#define INCLUDED_GR_PEAK_DETECTOR2_FB_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief Detect the peak of a signal
 * \ingroup peak_detectors_blk
 *
 * \details
 * If a peak is detected, this block outputs a 1, or it outputs
 * 0's. A separate debug output may be connected, to view the
 * internal estimated mean described below.
 */
class BLOCKS_API peak_detector2_fb : virtual public sync_block
{
public:
    // gr::blocks::peak_detector2_fb::sptr
    typedef std::shared_ptr<peak_detector2_fb> sptr;

    /*!
     * Build a peak detector block with float in, byte out.
     *
     * \param threshold_factor_rise The threshold factor determines
     *        when a peak is present. An average of the input signal
     *        is calculated (through a single-pole autoregressive
     *        filter) and when the value of the input signal goes
     *        over threshold_factor_rise*average, we assume we are
     *        in the neighborhood of a peak. The block will then
     *        find the position of the maximum within a window of
     *        look_ahead samples starting at the point where the
     *        threshold was crossed upwards.
     * \param look_ahead The look-ahead value is used when the
     *        threshold is crossed upwards to locate the peak within
     *        this range.
     * \param alpha One minus the pole of a single-pole
     *        autoregressive filter that evaluates the average of
     *        the input signal.
     */
    static sptr
    make(float threshold_factor_rise = 7, int look_ahead = 1000, float alpha = 0.001);

    /*! \brief Set the threshold factor value for the rise time
     *  \param thr new threshold factor
     */
    virtual void set_threshold_factor_rise(float thr) = 0;

    /*! \brief Set the look-ahead factor
     *  \param look new look-ahead factor
     */
    virtual void set_look_ahead(int look) = 0;

    /*! \brief Set the running average alpha
     *  \param alpha new alpha for running average
     */
    virtual void set_alpha(float alpha) = 0;

    /*! \brief Get the threshold factor value for the rise time
     *  \return threshold factor
     */
    virtual float threshold_factor_rise() = 0;

    /*! \brief Get the look-ahead factor value
     *  \return look-ahead factor
     */
    virtual int look_ahead() = 0;

    /*! \brief Get the alpha value of the running average
     *  \return alpha
     */
    virtual float alpha() = 0;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_PEAK_DETECTOR2_FB_H */
