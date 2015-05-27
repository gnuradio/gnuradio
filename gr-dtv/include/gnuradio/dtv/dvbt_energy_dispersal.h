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

#ifndef INCLUDED_DTV_DVBT_ENERGY_DISPERSAL_H
#define INCLUDED_DTV_DVBT_ENERGY_DISPERSAL_H

#include <gnuradio/dtv/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace dtv {

    /*!
     * \brief Energy dispersal.
     * \ingroup dtv
     *
     * ETSI EN 300 744 - Clause 4.3.1 \n
     * Input - MPEG-2 transport packets (including sync - 0x47). \n
     * Output - Randomized MPEG-2 transport packets. \n
     * If first byte is not a SYNC then look for it. \n
     * First sync in a row of 8 packets is reversed - 0xB8. \n
     * Block size is 188 bytes.
     */
    class DTV_API dvbt_energy_dispersal : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<dvbt_energy_dispersal> sptr;

      /*!
       * \brief Create DVB-T energy dispersal.
       *
       * \param nsize number of blocks.
       */
      static sptr make(int nsize);
    };

  } // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT_ENERGY_DISPERSAL_H */

