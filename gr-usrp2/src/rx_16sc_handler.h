/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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

#ifndef INCLUDED_RX_16SC_HANDLER_H
#define INCLUDED_RX_16SC_HANDLER_H

#include <usrp2/rx_nop_handler.h>
#include <usrp2/copiers.h>

#define RX_16SC_HANDLER_DEBUG 0

class rx_16sc_handler : public usrp2::rx_nop_handler
{
  std::complex<int16_t> *d_dest;

  // Private constructor
  rx_16sc_handler(uint64_t max_samples, uint64_t max_quantum, std::complex<int16_t> *dest)
    : rx_nop_handler(max_samples, max_quantum), d_dest(dest) {}

public:
  // Shared pointer to one of these
  typedef boost::shared_ptr<rx_16sc_handler> sptr;

  // Factory function to return a shared pointer to a new instance
  static sptr make(uint64_t max_samples, uint64_t max_quantum, std::complex<int16_t> *dest) 
  {
    if (RX_16SC_HANDLER_DEBUG)
      printf("rx_16sc_handler: max_samples=%li max_quantum=%li\n", max_samples, max_quantum);
      
    return sptr(new rx_16sc_handler(max_samples, max_quantum, dest));
  }

  // Invoked by USRP2 API when samples are available
  bool operator()(const uint32_t *items, size_t nitems, const usrp2::rx_metadata *metadata)
  {
    if (RX_16SC_HANDLER_DEBUG)
      printf("rx_16sc_handler: called with items=%zu ", nitems);

    // Copy/reformat/endian swap USRP2 data to destination buffer
    usrp2::copy_u2_16sc_to_host_16sc(nitems, items, d_dest);
    d_dest += nitems;

    // FIXME: do something with metadata

    // Determine if there is room to be called again
    bool ok = rx_nop_handler::operator()(items, nitems, metadata);
    if (RX_16SC_HANDLER_DEBUG)
      printf("ok to call again=%i\n", ok);

    return ok;
  }

  ~rx_16sc_handler();
};

#endif /* INCLUDED_RX_16SC_HANDLER_H */
