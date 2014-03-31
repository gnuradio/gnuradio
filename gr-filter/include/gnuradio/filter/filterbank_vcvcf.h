/* -*- c++ -*- */
/*
 * Copyright 2009,2010,2012,2014 Free Software Foundation, Inc.
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


#ifndef INCLUDED_FILTER_FILTERBANK_CHANNELIZER_VCVCF_H
#define	INCLUDED_FILTER_FILTERBANK_CHANNELIZER_VCVCF_H

#include <gnuradio/filter/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace filter {

    /*!
     * \brief Filterbank with vector of gr_complex input,
     * vector of gr_complex output and float taps
     * \ingroup filter_blk
     *
     * \details
     * This block takes in complex vectors and outputs complex vectors of the same
     * size.  Vectors of length N, rather than N normal streams are used to reduce
     * overhead.
     */
    class FILTER_API filterbank_vcvcf : virtual public block
    {
    public:
      typedef boost::shared_ptr<filterbank_vcvcf> sptr;

      /*!
       * Build the filterbank.
       * \param taps (vector of vector of floats/list of list of floats)
       *             Used to populate the filters.
       */
      static sptr make(const std::vector< std::vector<float> > &taps);

      /*!
       * Resets the filterbank's filter taps with the new prototype filter
       * \param taps (vector/list of floats) The prototype filter to populate the filterbank.
       */
      virtual void set_taps(const std::vector< std::vector<float> > &taps) = 0;

      /*!
       * Print all of the filterbank taps to screen.
       */
      virtual void print_taps() = 0;

      /*!
       * Return a vector<vector<>> of the filterbank taps
       */
      virtual std::vector<std::vector<float> > taps() const = 0;

    };

  } /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_FILTER_PFB_CHANNELIZER_VCVCF_H */
