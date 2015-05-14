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

#ifndef INCLUDED_DTV_DVBT_CONVOLUTIONAL_INTERLEAVER_H
#define INCLUDED_DTV_DVBT_CONVOLUTIONAL_INTERLEAVER_H

#include <gnuradio/dtv/api.h>
#include <gnuradio/sync_interpolator.h>

namespace gr {
  namespace dtv {

    /*!
     * \brief Convolutional interleaver.
     * \ingroup dtv
     *
     * ETSI EN 300 744 Clause 4.3.1 \n
     * Forney (Ramsey type III) convolutional interleaver. \n
     * Input: Blocks of I bytes size. \n
     * Output: Stream of 1 byte elements.
     */
    class DTV_API dvbt_convolutional_interleaver : virtual public sync_interpolator
    {
     public:
      typedef boost::shared_ptr<dvbt_convolutional_interleaver> sptr;

      /*!
       * \brief Create a DVB-T convolutional interleaver.
       *
       * \param nsize number of blocks to process. \n
       * \param I size of a block. \n
       * \param M depth length for each element in shift registers.
       */
      static sptr make(int nsize, int I, int M);
    };

  } // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT_CONVOLUTIONAL_INTERLEAVER_H */

