/* -*- c++ -*- */
/*
 * Copyright 2002 Free Software Foundation, Inc.
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

#ifndef _ATSC_REED_SOLOMON_H_
#define _ATSC_REED_SOLOMON_H_

#include <gnuradio/atsc/api.h>
#include <gnuradio/atsc/types.h>

/*!
 * \brief ATSC Reed-Solomon encoder / decoder
 *
 * The t=10 (207,187) code described in ATSC standard A/53B.
 * See figure D5 on page 55.
 */

class ATSC_API atsci_reed_solomon {

 public:
  atsci_reed_solomon();
  ~atsci_reed_solomon();

  /*!
   * \brief Add RS error correction encoding
   */
  void encode (atsc_mpeg_packet_rs_encoded &out, const atsc_mpeg_packet_no_sync &in);

  /*!
   * Decode RS encoded packet.
   * \returns a count of corrected symbols, or -1 if the block was uncorrectible.
   */
  int decode (atsc_mpeg_packet_no_sync &out, const atsc_mpeg_packet_rs_encoded &in);

 private:
  void	*d_rs;
};

#endif /* _ATSC_REED_SOLOMON_H_ */
