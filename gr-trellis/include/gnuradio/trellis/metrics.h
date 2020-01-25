/* -*- c++ -*- */
/*
 * Copyright 2004,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef METRICS_H
#define METRICS_H

#include <gnuradio/block.h>
#include <gnuradio/trellis/api.h>
#include <gnuradio/trellis/calc_metric.h>
#include <cstdint>

namespace gr {
namespace trellis {

/*!
 * \brief Evaluate metrics for use by the Viterbi algorithm.
 * \ingroup trellis_coding_blk
 */
template <class T>
class TRELLIS_API metrics : virtual public block
{
public:
    // gr::trellis::metrics::sptr
    typedef boost::shared_ptr<metrics<T>> sptr;

    static sptr
    make(int O, int D, const std::vector<T>& TABLE, digital::trellis_metric_type_t TYPE);

    virtual int O() const = 0;
    virtual int D() const = 0;
    virtual digital::trellis_metric_type_t TYPE() const = 0;
    virtual std::vector<T> TABLE() const = 0;

    virtual void set_O(int O) = 0;
    virtual void set_D(int D) = 0;
    virtual void set_TYPE(digital::trellis_metric_type_t type) = 0;
    virtual void set_TABLE(const std::vector<T>& table) = 0;
};


typedef metrics<std::int16_t> metrics_s;
typedef metrics<std::int32_t> metrics_i;
typedef metrics<float> metrics_f;
typedef metrics<gr_complex> metrics_c;
} /* namespace trellis */
} /* namespace gr */

#endif /* METRICS_H */
