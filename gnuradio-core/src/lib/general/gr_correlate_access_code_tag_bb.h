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

#ifndef INCLUDED_gr_correlate_access_code_tag_bb_H
#define INCLUDED_gr_correlate_access_code_tag_bb_H

#include <gr_core_api.h>
#include <gr_sync_block.h>
#include <string>

class gr_correlate_access_code_tag_bb;
typedef boost::shared_ptr<gr_correlate_access_code_tag_bb> gr_correlate_access_code_tag_bb_sptr;

/*!
 * \param access_code is represented with 1 byte per bit, e.g., "010101010111000100"
 * \param threshold maximum number of bits that may be wrong
 * \param tag_name key of the tag inserted into the tag stream
 */
GR_CORE_API gr_correlate_access_code_tag_bb_sptr 
gr_make_correlate_access_code_tag_bb (const std::string &access_code, int threshold,
				      const std::string &tag_name);

/*!
 * \brief Examine input for specified access code, one bit at a time.
 * \ingroup sync_blk
 *
 * input:  stream of bits, 1 bit per input byte (data in LSB)
 * output: unaltered stream of bits (plus tags)
 *
 * This block annotates the input stream with tags. The tags have key
 * name [tag_name], specified in the constructor. Used for searching
 * an input data stream for preambles, etc.
 */
class GR_CORE_API gr_correlate_access_code_tag_bb : public gr_sync_block
{
  friend GR_CORE_API gr_correlate_access_code_tag_bb_sptr 
  gr_make_correlate_access_code_tag_bb (const std::string &access_code, int threshold,
					const std::string &tag_name);
 private:
  unsigned long long d_access_code;	// access code to locate start of packet
                                        //   access code is left justified in the word
  unsigned long long d_data_reg;	// used to look for access_code
  unsigned long long d_mask;		// masks access_code bits (top N bits are set where
                                        //   N is the number of bits in the access code)
  unsigned int	     d_threshold;	// how many bits may be wrong in sync vector
  unsigned int       d_len;         //the length of the access code

  pmt::pmt_t d_key, d_me; //d_key is the tag name, d_me is the block name + unique ID

 protected:
  gr_correlate_access_code_tag_bb(const std::string &access_code, int threshold,
				  const std::string &tag_name);

 public:
  ~gr_correlate_access_code_tag_bb();

  int work(int noutput_items,
	   gr_vector_const_void_star &input_items,
	   gr_vector_void_star &output_items);

  
  /*!
   * \param access_code is represented with 1 byte per bit, e.g., "010101010111000100"
   */
  bool set_access_code (const std::string &access_code);
};

#endif /* INCLUDED_gr_correlate_access_code_tag_bb_H */
