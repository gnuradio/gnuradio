/* -*- c++ -*- */
/*
 * Copyright 2012-2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
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
       typedef boost::shared_ptr<plateau_detector_fb> sptr;

       /*!
	* \param max_len Maximum length of the plateau
	* \param threshold Anything above this value is considered a plateau
	*/
       static sptr make(int max_len, float threshold=0.9);

       virtual void set_threshold(float threshold) = 0;
       virtual float threshold() const = 0;
    };

  } // namespace blocks
} // namespace gr

#endif /* INCLUDED_BLOCKS_PLATEAU_DETECTOR_FB_H */

