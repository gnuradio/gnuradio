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

#ifndef INCLUDED_DTV_DVBT_REFERENCE_SIGNALS_H
#define INCLUDED_DTV_DVBT_REFERENCE_SIGNALS_H

#include <gnuradio/dtv/api.h>
#include <gnuradio/dtv/dvb_config.h>
#include <gnuradio/dtv/dvbt_config.h>
#include <gnuradio/block.h>

namespace gr {
  namespace dtv {

    /*!
     * \brief Reference signals generator.
     * \ingroup dtv
     *
     * ETSI EN 300 744 Clause 4.5 \n
     * Data input format: \n
     * complex(real(float), imag(float)). \n
     * Data output format: \n
     * complex(real(float), imag(float)).
     */
    class DTV_API dvbt_reference_signals : virtual public block
    {
     public:
      typedef boost::shared_ptr<dvbt_reference_signals> sptr;

      /*!
       * \brief Create Reference signals generator.
       *
       * \param itemsize size of an in/out item. \n
       * \param ninput input stream length. \n
       * \param noutput output stream length. \n
       * \param constellation constellation used. \n
       * \param hierarchy hierarchy used. \n
       * \param code_rate_HP high priority stream code rate. \n
       * \param code_rate_LP low priority stream code rate. \n
       * \param guard_interval guard interval used. \n
       * \param transmission_mode transmission mode used. \n
       * \param include_cell_id include or not Cell ID. \n
       * \param cell_id value of the Cell ID.
       */
      static sptr make(int itemsize, int ninput, int noutput, dvb_constellation_t constellation, \
           dvbt_hierarchy_t hierarchy, dvb_code_rate_t code_rate_HP, dvb_code_rate_t code_rate_LP, \
           dvb_guardinterval_t guard_interval, dvbt_transmission_mode_t transmission_mode, int include_cell_id, int cell_id);
    };

  } // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT_REFERENCE_SIGNALS_H */

