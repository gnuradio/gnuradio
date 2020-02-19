/* -*- c++ -*- */
/*
 * Copyright 2004,2010-2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_TRELLIS_CONSTELLATION_METRICS_CF_H
#define INCLUDED_TRELLIS_CONSTELLATION_METRICS_CF_H

#include <gnuradio/block.h>
#include <gnuradio/digital/constellation.h>
#include <gnuradio/digital/metric_type.h>
#include <gnuradio/trellis/api.h>

namespace gr {
namespace trellis {

/*!
 * \brief Evaluate metrics for use by the Viterbi algorithm.
 * \ingroup trellis_coding_blk
 */
class TRELLIS_API constellation_metrics_cf : virtual public block
{
public:
    // gr::trellis::constellation_metrics_cf::sptr
    typedef boost::shared_ptr<constellation_metrics_cf> sptr;

    static sptr make(digital::constellation_sptr constellation,
                     digital::trellis_metric_type_t TYPE);
};

} /* namespace trellis */
} /* namespace gr */

#endif /* INCLUDED_TRELLIS_CONSTELLATION_METRICS_CF_H */
