/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef INCLUDED_USRP2_RX_SAMPLE_HANDLER_H
#define INCLUDED_USRP2_RX_SAMPLE_HANDLER_H

#include <usrp2/metadata.h>
#include <stddef.h>


namespace usrp2 {

  /*!
   * \brief Abstract function object called to handle received data blocks.
   * \ingroup usrp2
   *
   * An object derived from this class is passed to usrp2::rx_samples
   * to process the received frames of samples.
   */
  class rx_sample_handler {
  public:
    virtual ~rx_sample_handler();

    /*!
     * \param items points to the first 32-bit word of uninterpreted sample data in the frame.
     * \param nitems is the number of entries in the frame in units of uint32_t's.
     * \param metadata is the additional per frame data provided by the USRP2 FPGA.
     *
     * \p items points to the raw sample data received off of the ethernet.  The data is
     * packed into big-endian 32-bit unsigned ints for transport, but the actual format
     * of the data is dependent on the current configuration of the USRP2.  The most common
     * format is 16-bit I & Q, with I in the top of the 32-bit word.
     *
     * This is the general purpose, low level interface and relies on other functions
     * to handle all required endian-swapping and format conversion.  \sa FIXME.
     *
     * \returns true if the object wants to be called again with new data;
     * false if no additional data is wanted.
     */
    virtual bool operator()(const uint32_t *items, size_t nitems, const rx_metadata *metadata) = 0;
  };

};

#endif /* INCLUDED_RX_SAMPLE_HANDLER_H */
