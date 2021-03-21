/* -*- c++ -*- */
/*
 * Copyright 2021 NTESS LLC.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_PDU_TIME_DELTA_H
#define INCLUDED_PDU_TIME_DELTA_H

#include <gnuradio/block.h>
#include <gnuradio/pdu/api.h>

namespace gr {
namespace pdu {

/*!
 * \brief Compute system time differences and provide statistics upon completion.
 * \ingroup debug_tools_blk
 * \ingroup pdu_blk
 *
 * This block computes the difference between the current system time and a defined PDU
 * key previously added (e.g.: by by the add_system_time block). This is useful for
 * benchmarking PDU operations. The block also maintians running estimates of mean and
 * variance, accessible by getters and printed when the flowgraph is stopped.
 */
class PDU_API time_delta : virtual public gr::block
{
public:
    typedef std::shared_ptr<time_delta> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of pdu::time_delta.
     *
     * @param delta_key - key to use for time delta metadata field
     * @param time_key - key to use as reference time for delta
     */
    static sptr make(const pmt::pmt_t delta_key, const pmt::pmt_t time_key);

    virtual double get_variance(void) = 0;
    virtual double get_mean(void) = 0;
    virtual void reset_stats(void) = 0;
};

} // namespace pdu
} // namespace gr

#endif /* INCLUDED_PDU_TIME_DELTA_H */
