/* -*- c++ -*- */
/* Copyright 2012 Free Software Foundation, Inc.
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


#ifndef INCLUDED_DIGITAL_CRC32_BB_H
#define INCLUDED_DIGITAL_CRC32_BB_H

#include <digital_api.h>
#include <gr_tagged_stream_block.h>

class digital_crc32_bb;

typedef boost::shared_ptr<digital_crc32_bb> digital_crc32_bb_sptr;

DIGITAL_API digital_crc32_bb_sptr digital_make_crc32_bb (bool check=false, const std::string& lengthtagname="packet_len");

/*!
 * \brief Byte-stream CRC block
 * \ingroup digital
 *
 * Input: stream of bytes, which form a packet. The first byte of the packet
 * has a tag with key "length" and the value being the number of bytes in the
 * packet.
 *
 * Output: The same bytes as incoming, but trailing a CRC32 of the packet.
 * The tag is re-set to the new length.
 */
class DIGITAL_API digital_crc32_bb : public gr_tagged_stream_block
{
 private:
  friend DIGITAL_API digital_crc32_bb_sptr digital_make_crc32_bb (bool check, const std::string& lengthtagname);

  bool d_check;

  digital_crc32_bb(bool check, const std::string& lengthtagname);

 public:
  ~digital_crc32_bb();

  int calculate_output_stream_length(const std::vector<int> &ninput_items);

  int work (int noutput_items,
      gr_vector_int &ninput_items,
      gr_vector_const_void_star &input_items,
      gr_vector_void_star &output_items);
};

#endif /* INCLUDED_DIGITAL_CRC32_BB_H */

