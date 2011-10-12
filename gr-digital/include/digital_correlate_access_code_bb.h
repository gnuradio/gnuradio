/* -*- c++ -*- */
/*
 * Copyright 2005,2006,2011 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_CORRELATE_ACCESS_CODE_BB_H
#define INCLUDED_DIGITAL_CORRELATE_ACCESS_CODE_BB_H

#include <digital_api.h>
#include <gr_sync_block.h>
#include <string>

class digital_correlate_access_code_bb;
typedef boost::shared_ptr<digital_correlate_access_code_bb> digital_correlate_access_code_bb_sptr;

/*!
 * \param access_code is represented with 1 byte per bit, e.g., "010101010111000100"
 * \param threshold maximum number of bits that may be wrong
 */
DIGITAL_API digital_correlate_access_code_bb_sptr 
digital_make_correlate_access_code_bb (const std::string &access_code, int threshold);

/*!
 * \brief Examine input for specified access code, one bit at a time.
 * \ingroup sync_blk
 * \ingroup digital
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
class DIGITAL_API digital_correlate_access_code_bb : public gr_sync_block
{
  friend DIGITAL_API digital_correlate_access_code_bb_sptr 
  digital_make_correlate_access_code_bb (const std::string &access_code, int threshold);
 private:
  unsigned long long d_access_code;	// access code to locate start of packet
                                        //   access code is left justified in the word
  unsigned long long d_data_reg;	// used to look for access_code
  unsigned long long d_flag_reg;	// keep track of decisions
  unsigned long long d_flag_bit;	// mask containing 1 bit which is location of new flag
  unsigned long long d_mask;		// masks access_code bits (top N bits are set where
                                        //   N is the number of bits in the access code)
  unsigned int	     d_threshold;	// how many bits may be wrong in sync vector

 protected:
  digital_correlate_access_code_bb(const std::string &access_code, int threshold);

 public:
  ~digital_correlate_access_code_bb();

  int work(int noutput_items,
	   gr_vector_const_void_star &input_items,
	   gr_vector_void_star &output_items);

  
  /*!
   * \param access_code is represented with 1 byte per bit, e.g., "010101010111000100"
   */
  bool set_access_code (const std::string &access_code);
};

#endif /* INCLUDED_DIGITAL_CORRELATE_ACCESS_CODE_BB_H */
