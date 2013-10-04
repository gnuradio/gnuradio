/* -*- c++ -*- */
/*
 * Copyright 2011,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_UHD_AMSG_SOURCE_H
#define INCLUDED_GR_UHD_AMSG_SOURCE_H

#include <uhd/usrp/multi_usrp.hpp>
#include <gnuradio/uhd/api.h>
#include <gnuradio/msg_queue.h>

namespace gr {
  namespace uhd {

    class uhd_amsg_source;

    class GR_UHD_API amsg_source
    {
    public:
      // gr::uhd::amsg_source::sptr
      typedef boost::shared_ptr<amsg_source> sptr;

      /*!
       * \brief Destructor
       *
       * This must be explicitly defined because there is no block inheritance
       * for this class, and thus no virtual destructor pulled into the class
       * hierarchy for sub-classes.
       */
      virtual ~amsg_source(){}

      /*!
       * \brief Make a new USRP asynchronous message-based source block.
       * \ingroup uhd_blk
       */
      static sptr make(const ::uhd::device_addr_t &device_addr,
                       msg_queue::sptr msgq);

      /*!
       * Convert a raw asynchronous message to an asynchronous metatdata object.
       * \return The asynchronous metadata object.
       */
      static ::uhd::async_metadata_t
        msg_to_async_metadata_t(const message::sptr msg);
    };

  } /* namespace uhd */
} /* namespace gr */

#endif /* INCLUDED_GR_UHD_AMSG_SOURCE_H */
