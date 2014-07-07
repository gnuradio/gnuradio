/* -*- c++ -*- */
/*
 * Copyright 2012,2013 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef INCLUDED_MSG_PRODUCER_H
#define INCLUDED_MSG_PRODUCER_H

#include <gnuradio/api.h>
#include <pmt/pmt.h>
#include <boost/shared_ptr.hpp>

namespace gr {
  namespace messages {

    /*!
     * \brief Virtual base class that produces messages
     */
    class GR_RUNTIME_API msg_producer
    {
    public:
      msg_producer() {}
      virtual ~msg_producer();

      /*!
       * \brief send \p msg to \p msg_producer
       */
      virtual pmt::pmt_t retrieve() = 0;
    };

    typedef boost::shared_ptr<msg_producer> msg_producer_sptr;

  } /* namespace messages */
} /* namespace gr */

#endif /* INCLUDED_MSG_PRODUCER_H */
