/* -*- c++ -*- */
/*
 * Copyright 2013,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio.
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

#ifndef INCLUDED_ZEROMQ_MSG_BASE_H
#define INCLUDED_ZEROMQ_MSG_BASE_H

#include <gnuradio/zeromq/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace zeromq {

    /*!
     * \brief Base class providing public API for message zeromq blocks
     * \ingroup zeromq
     */
    class ZEROMQ_API msg_base : public gr::block
    {
    protected:
      /*
       * Required for virtual inheritance
       */
      msg_base() {};
      /*
       * Actual ctor passed through to block
       */
      msg_base(const std::string &name,
               gr::io_signature::sptr input_signature,
               gr::io_signature::sptr output_signature) :
          block(name, input_signature, output_signature) {};

    public:
    /*!
     * \brief Return the endpoint address
     */
    virtual std::string endpoint() = 0;

    /*!
     * \brief Set the zeromq endpoint
     *
     * \param address the endpoint address
     */
    virtual void set_endpoint(const char* address) = 0;

  };

  } // namespace zeromq
} // namespace gr

#endif /* INCLUDED_ZEROMQ_MSG_BASE_H */
