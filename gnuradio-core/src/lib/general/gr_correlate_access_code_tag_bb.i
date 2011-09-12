/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
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

GR_SWIG_BLOCK_MAGIC(gr,correlate_access_code_tag_bb);

/*!
 * \param access_code is represented with 1 byte per bit, e.g., "010101010111000100"
 * \param threshold maximum number of bits that may be wrong
 */
gr_correlate_access_code_tag_bb_sptr 
gr_make_correlate_access_code_tag_bb (const std::string &access_code, int threshold, const std::string &tag_name) 
  throw(std::out_of_range);

/*!
 * \brief Examine input for specified access code, one bit at a time.
 * \ingroup block
 *
 * input:  stream of bits, 1 bit per input byte (data in LSB)
 * output: stream of bits, 2 bits per output byte (data in LSB, flag in next higher bit)
 *
 * Each output byte contains two valid bits, the data bit, and the
 * flag bit.  The LSB (bit 0) is the data bit, and is the original
 * input data, delayed 64 bits.  Bit 1 is the
 * flag bit and is 1 if the corresponding data bit is the first data
 * bit following the access code. Otherwise the flag bit is 0.
 */
class gr_correlate_access_code_tag_bb : public gr_sync_block
{
  friend gr_correlate_access_code_tag_bb_sptr 
  gr_make_correlate_access_code_tag_bb (const std::string &access_code, int threshold, const std::string &tag_name);
 protected:
  gr_correlate_access_code_tag_bb(const std::string &access_code, int threshold, const std::string &tag_name);

 public:
  ~gr_correlate_access_code_tag_bb();

  /*!
   * \param access_code is represented with 1 byte per bit, e.g., "010101010111000100"
   */
  bool set_access_code (const std::string &access_code);
};
