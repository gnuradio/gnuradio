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

#ifndef INCLUDED_DTV_ATSC_FIELD_SYNC_MUX_H
#define INCLUDED_DTV_ATSC_FIELD_SYNC_MUX_H

#include <gnuradio/dtv/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace dtv {

    /*!
     * \brief <+description of block+>
     * \ingroup dtv
     *
     */
    class DTV_API atsc_field_sync_mux : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<atsc_field_sync_mux> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of dtv::atsc_field_sync_mux.
       */
      static sptr make();
    };

  } // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_ATSC_FIELD_SYNC_MUX_H */
