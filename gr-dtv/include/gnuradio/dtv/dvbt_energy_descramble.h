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

#ifndef INCLUDED_DTV_DVBT_ENERGY_DESCRAMBLE_H
#define INCLUDED_DTV_DVBT_ENERGY_DESCRAMBLE_H

#include <gnuradio/dtv/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace dtv {

    /*!
     * \brief Energy descramble
     * \ingroup dtv
     *
     * ETSI EN 300 744 - Clause 4.3.1. \n
     * Input - Randomized MPEG-2 transport packets. \n
     * Output - MPEG-2 transport packets (including sync - 0x47). \n
     * We assume the first byte is a NSYNC. \n
     * First sync in a row of 8 packets is reversed - 0xB8. \n
     * Block size is 188 bytes
     */
    class DTV_API dvbt_energy_descramble : virtual public block
    {
     public:
      typedef boost::shared_ptr<dvbt_energy_descramble> sptr;

      /*!
       * \brief Create DVB-T Energy descramble.
       *
       * \param nblocks number of blocks.
       */
      static sptr make(int nblocks);
    };

  } // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT_ENERGY_DESCRAMBLE_H */

