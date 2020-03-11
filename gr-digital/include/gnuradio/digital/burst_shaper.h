/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef BURST_SHAPER_H
#define BURST_SHAPER_H

#include <gnuradio/block.h>
#include <gnuradio/digital/api.h>

namespace gr {
namespace digital {

/*!
 * \brief Burst shaper block for applying burst padding and ramping.
 * \ingroup packet_operators_blk
 *
 * \details
 *
 * This block applies a configurable amount of zero padding before
 * and/or after a burst indicated by tagged stream length tags.
 *
 * If phasing symbols are used, an alternating pattern of +1/-1
 * symbols of length ceil(N/2) will be inserted before and after
 * each burst, where N is the length of the taps vector. The ramp-
 * up/ramp-down shape will be applied to these phasing symbols.
 *
 * If phasing symbols are not used, the taper will be applied
 * directly to the head and tail of each burst.
 *
 * Length tags will be updated to include the length of any added
 * zero padding or phasing symbols and will be placed at the
 * beginning of the modified tagged stream. Any other tags found at
 * the same offset as a length tag will also be placed at the
 * beginning of the modified tagged stream, since these tags are
 * assumed to be associated with the burst rather than a specific
 * sample. For example, if "tx_time" tags are used to control
 * bursts, their offsets should be consistent with their associated
 * burst's length tags. Tags at other offsets will be placed with
 * the samples on which they were found.
 *
 * \li input: stream of T
 * \li output: stream of T
 */
template <class T>
class DIGITAL_API burst_shaper : virtual public block
{
public:
    typedef boost::shared_ptr<burst_shaper<T>> sptr;

    /*!
     * Make a burst shaper block.
     *
     * \param taps: vector of window taper taps; the first ceil(N/2)
     *              items are the up flank and the last ceil(N/2)
     *              items are the down flank. If taps.size() is odd,
     *              the middle tap will be used as the last item of
     *              the up flank and first item of the down flank.
     * \param pre_padding: number of zero samples to insert before
     *                     the burst.
     * \param post_padding: number of zero samples to append after
     *                      the burst.
     * \param insert_phasing: if true, insert alternating +1/-1
     *                        pattern of length ceil(N/2) before and
     *                        after the burst and apply ramp up and
     *                        ramp down taps, respectively, to the
     *                        inserted patterns instead of the head
     *                        and tail items of the burst.
     * \param length_tag_name: the name of the tagged stream length
     *                         tag key.
     */
    static sptr make(const std::vector<T>& taps,
                     int pre_padding = 0,
                     int post_padding = 0,
                     bool insert_phasing = false,
                     const std::string& length_tag_name = "packet_len");

    /*!
     * Returns the amount of zero padding inserted before each burst.
     */
    virtual int pre_padding() const = 0;

    /*!
     * Returns the amount of zero padding inserted after each burst.
     */
    virtual int post_padding() const = 0;

    /*!
     * Returns the total amount of zero padding and phasing symbols
     * inserted before each burst.
     */
    virtual int prefix_length() const = 0;

    /*!
     * Returns the total amount of zero padding and phasing symbols
     * inserted after each burst.
     */
    virtual int suffix_length() const = 0;
};

typedef burst_shaper<float> burst_shaper_ff;
typedef burst_shaper<gr_complex> burst_shaper_cc;
} // namespace digital
} // namespace gr

#endif /* BURST_SHAPER_H */
