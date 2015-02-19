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

#ifndef INCLUDED_DTV_DVB_LDPC_BB_H
#define INCLUDED_DTV_DVB_LDPC_BB_H

#include <gnuradio/dtv/api.h>
#include <gnuradio/dtv/dvb_config.h>
#include <gnuradio/block.h>

namespace gr {
  namespace dtv {

    /*!
     * \brief <+description of block+>
     * \ingroup dtv
     *
     */
    class DTV_API dvb_ldpc_bb : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<dvb_ldpc_bb> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of dtv::dvb_ldpc_bb.
       */
      static sptr make(dvb_standard_t standard, dvb_framesize_t framesize, dvb_code_rate_t rate, dvb_constellation_t constellation);
    };

  } // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVB_LDPC_BB_H */

