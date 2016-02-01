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

#ifndef INCLUDED_DTV_DVBT_CONVOLUTIONAL_DEINTERLEAVER_H
#define INCLUDED_DTV_DVBT_CONVOLUTIONAL_DEINTERLEAVER_H

#include <gnuradio/dtv/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace dtv {

    /*!
     * \brief Convolutional deinterleaver.
     * \ingroup dtv
     *
     * ETSI EN 300 744 Clause 4.3.1 \n
     * Forney (Ramsey type III) convolutional deinterleaver. \n
     * Data input: Stream of 1 byte elements. \n
     * Data output: Blocks of I bytes size.
     */
    class DTV_API dvbt_convolutional_deinterleaver : virtual public block
    {
     public:
      typedef boost::shared_ptr<dvbt_convolutional_deinterleaver> sptr;

      /*!
       * \brief Create a DVB-T convolutional deinterleaver.
       *
       * \param nsize number of blocks to process. \n
       * \param I size of a block. \n
       * \param M depth length for each element in shift registers.
       */
      static sptr make(int nsize, int I, int M);
    };

  } // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT_CONVOLUTIONAL_DEINTERLEAVER_H */

