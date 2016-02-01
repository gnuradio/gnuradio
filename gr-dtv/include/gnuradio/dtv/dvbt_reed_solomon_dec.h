/* -*- c++ -*- */
/* 
 * Copyright 2015 Free Software Foundation, Inc.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_DTV_DVBT_REED_SOLOMON_DEC_H
#define INCLUDED_DTV_DVBT_REED_SOLOMON_DEC_H

#include <gnuradio/dtv/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace dtv {

    /*!
     * \brief Reed Solomon decoder.
     * \ingroup dtv
     *
     * ETSI EN 300 744 Clause 4.3.2 \n
     * RS(N=204,K=239,T=8).
     */
    class DTV_API dvbt_reed_solomon_dec : virtual public block
    {
     public:
      typedef boost::shared_ptr<dvbt_reed_solomon_dec> sptr;

      /*!
       * \brief Create a Reed Solomon decoder.
       *
       * \param p characteristic of GF(p^m).
       * \param m we use GF(p^m).
       * \param gfpoly Generator Polynomial.
       * \param n length of codeword of RS coder.
       * \param k length of information sequence of RS decoder.
       * \param t number of corrected errors.
       * \param s shortened length.
       * \param blocks number of blocks to process at once.
       */
      static sptr make(int p, int m, int gfpoly, int n, int k, int t, int s, int blocks);
    };

  } // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT_REED_SOLOMON_DEC_H */

