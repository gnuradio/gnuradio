/* -*- c++ -*- */
/*
 * Copyright 2004,2010-2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_TRELLIS_CONSTELLATION_METRICS_CF_H
#define INCLUDED_TRELLIS_CONSTELLATION_METRICS_CF_H

#include <gnuradio/trellis/api.h>
#include <gnuradio/block.h>
#include <gnuradio/digital/constellation.h>
#include <gnuradio/digital/metric_type.h>

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
