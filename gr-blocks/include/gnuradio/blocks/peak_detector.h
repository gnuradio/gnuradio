/* -*- c++ -*- */
/*
 * Copyright 2007,2013,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef PEAK_DETECTOR_H
#define PEAK_DETECTOR_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>
#include <cstdint>

namespace gr {
namespace blocks {

/*!
 * \brief Detect the peak of a signal
 * \ingroup peak_detectors_blk
 *
 * \details
 * If a peak is detected, this block outputs a 1,
 * or it outputs 0's.
 */
template <class T>
class BLOCKS_API peak_detector : virtual public sync_block
{
public:
    typedef std::shared_ptr<peak_detector<T>> sptr;

    /*!
     * Make a peak detector block.
     *
     * \param threshold_factor_rise The threshold factor determines
     *        when a peak has started. An average of the signal is
     *        calculated and when the value of the signal goes over
     *        threshold_factor_rise*average, we start looking for a
     *        peak.
     * \param threshold_factor_fall The threshold factor determines
     *        when a peak has ended. An average of the signal is
     *        calculated and when the value of the signal goes
     *        below threshold_factor_fall*average, we stop looking
     *        for a peak.
     * \param look_ahead The look-ahead value is used when the
     *        threshold is found to look if there another peak
     *        within this step range. If there is a larger value,
     *        we set that as the peak and look ahead again. This is
     *        continued until the highest point is found with This
     *        look-ahead range.
     * \param alpha The gain value of a moving average filter
     */
    static sptr make(float threshold_factor_rise = 0.25,
                     float threshold_factor_fall = 0.40,
                     int look_ahead = 10,
                     float alpha = 0.001);

    /*! \brief Set the threshold factor value for the rise time
     *  \param thr new threshold factor
     */
    virtual void set_threshold_factor_rise(float thr) = 0;

    /*! \brief Set the threshold factor value for the fall time
     *  \param thr new threshold factor
     */
    virtual void set_threshold_factor_fall(float thr) = 0;

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

    /*! \brief Get the threshold factor value for the fall time
     *  \return threshold factor
     */
    virtual float threshold_factor_fall() = 0;

    /*! \brief Get the look-ahead factor value
     *  \return look-ahead factor
     */
    virtual int look_ahead() = 0;

    /*! \brief Get the alpha value of the running average
     *  \return alpha
     */
    virtual float alpha() = 0;
};

typedef peak_detector<float> peak_detector_fb;
typedef peak_detector<std::int32_t> peak_detector_ib;
typedef peak_detector<std::int16_t> peak_detector_sb;

} /* namespace blocks */
} /* namespace gr */

#endif /* PEAK_DETECTOR_H */
