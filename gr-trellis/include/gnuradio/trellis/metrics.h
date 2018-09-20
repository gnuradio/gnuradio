/* -*- c++ -*- */
/*
 * Copyright 2004,2012,2018 Free Software Foundation, Inc.
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


#ifndef METRICS_H
#define METRICS_H

#include <gnuradio/trellis/api.h>
#include <gnuradio/trellis/calc_metric.h>
#include <gnuradio/block.h>
#include <cstdint>

namespace gr {
  namespace trellis {

    /*!
     * \brief Evaluate metrics for use by the Viterbi algorithm.
     * \ingroup trellis_coding_blk
     */
template<class T>
    class TRELLIS_API metrics : virtual public block
    {
    public:
      // gr::trellis::metrics::sptr
      typedef boost::shared_ptr< metrics<T> > sptr;

      static sptr make(int O, int D, const std::vector<T> &TABLE,
		       digital::trellis_metric_type_t TYPE);

      virtual int O() const = 0;
      virtual int D() const = 0;
      virtual digital::trellis_metric_type_t TYPE() const = 0;
      virtual std::vector<T> TABLE() const = 0;

      virtual void set_O(int O) = 0;
      virtual void set_D(int D) = 0;
      virtual void set_TYPE(digital::trellis_metric_type_t type) = 0;
      virtual void set_TABLE(const std::vector<T> &table) = 0;
    };


typedef metrics<std::int16_t> metrics_s;
typedef metrics<std::int32_t> metrics_i;
typedef metrics<float> metrics_f;
typedef metrics<gr_complex> metrics_c;
  } /* namespace trellis */
} /* namespace gr */

#endif /* METRICS_H */
