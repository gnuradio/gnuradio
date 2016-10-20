/* -*- c++ -*- */
/*
 * Copyright 2016 Free Software Foundation, Inc.
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

#ifndef GNURADIO_ZMQ_STREAM_BASE_H
#define GNURADIO_ZMQ_STREAM_BASE_H

#include <gnuradio/zeromq/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
    namespace zeromq {

      /*!
       * \brief Base class providing public API for streaming zeromq blocks
       * \ingroup zeromq
       */
      class ZEROMQ_API stream_base : public gr::sync_block
      {
      protected:
        /*
         * Required for virtual inheritance
         */
        stream_base() {};
        /*
         * The actual ctor that passes through to sync_block
         */
        stream_base(const std::string &name,
                    gr::io_signature::sptr input_signature,
                    gr::io_signature::sptr output_signature) :
            sync_block(name, input_signature, output_signature) {};

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




#endif //GNURADIO_ZMQ_STREAM_BASE_H
