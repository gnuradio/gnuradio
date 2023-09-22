/* -*- c++ -*- */
/*
 * Copyright 2012-2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef INCLUDED_BLOCKS_PLATEAU_DETECTOR_FB_H
#define INCLUDED_BLOCKS_PLATEAU_DETECTOR_FB_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief Detects a plateau and marks the middle.
 * \ingroup peak_detectors_blk
 *
 * \details
 * Detect a plateau of a-priori known height. Input is a stream of floats,
 * the output is a stream of bytes. Whenever a plateau is detected, the
 * middle of that plateau is marked with a '1' on the output stream (all
 * other samples are left at zero).
 *
 * You can use this in a Schmidl & Cox synchronisation algorithm to interpret
 * the output of the normalized correlator. Just pass the length of the cyclic
 * prefix (in samples) as the max_len parameter).
 *
 * Unlike the peak detectors, you must the now the absolute height of the plateau.
 * Whenever the amplitude exceeds the given threshold, it starts assuming the
 * presence of a plateau.
 *
 * An implicit hysteresis is provided by the fact that after detecting one plateau,
 * it waits at least max_len samples before the next plateau can be detected.
 */
class BLOCKS_API plateau_detector_fb : virtual public block
{
public:
    typedef std::shared_ptr<plateau_detector_fb> sptr;

    /*!
     * \param max_len Maximum length of the plateau
     * \param threshold Anything above this value is considered a plateau
     */
    static sptr make(int max_len, float threshold = 0.9);

    virtual void set_threshold(float threshold) = 0;
    virtual float threshold() const = 0;
};

} // namespace blocks
} // namespace gr

#endif /* INCLUDED_BLOCKS_PLATEAU_DETECTOR_FB_H */
