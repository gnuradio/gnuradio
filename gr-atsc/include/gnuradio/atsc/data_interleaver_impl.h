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

#ifndef _ATSC_DATA_INTERLEAVER_H_
#define _ATSC_DATA_INTERLEAVER_H_

#include <gnuradio/atsc/api.h>
#include <gnuradio/atsc/types.h>
#include <gnuradio/atsc/convolutional_interleaver.h>

/*!
 * \brief atsc convolutional data interleaver
 */
class ATSC_API atsci_data_interleaver : public convolutional_interleaver<unsigned char> {
 public:
  atsci_data_interleaver () : convolutional_interleaver<unsigned char>(true, 52, 4) {}

  void interleave (atsc_mpeg_packet_rs_encoded &out,
		   const atsc_mpeg_packet_rs_encoded &in);
};

/*!
 * \brief atsc convolutional data deinterleaver
 */
class ATSC_API atsci_data_deinterleaver : public convolutional_interleaver<unsigned char> {
 public:
  atsci_data_deinterleaver () :
    convolutional_interleaver<unsigned char>(false, 52, 4), alignment_fifo (156) {}

  void deinterleave (atsc_mpeg_packet_rs_encoded &out,
		     const atsc_mpeg_packet_rs_encoded &in);

private:
  /*!
   * Note: The use of the alignment_fifo keeps the encoder and decoder
   * aligned if both are synced to a field boundary.  There may be other
   * ways to implement this function.  This is a best guess as to how
   * this should behave, as we have no test vectors for either the
   * interleaver or deinterleaver.
   */
  interleaver_fifo<unsigned char> alignment_fifo;

  static void remap_pli (plinfo &out, const plinfo &in);
};

#endif /* _ATSC_DATA_INTERLEAVER_H_ */
