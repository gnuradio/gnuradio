/* -*- c++ -*- */
/*
 * Copyright 2006,2007,2011 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_OFDM_MAPPER_BCV_H
#define INCLUDED_DIGITAL_OFDM_MAPPER_BCV_H

#include <digital_api.h>
#include <gr_sync_block.h>
#include <gr_message.h>
#include <gr_msg_queue.h>

class digital_ofdm_mapper_bcv;
typedef boost::shared_ptr<digital_ofdm_mapper_bcv> digital_ofdm_mapper_bcv_sptr;

DIGITAL_API digital_ofdm_mapper_bcv_sptr 
digital_make_ofdm_mapper_bcv (const std::vector<gr_complex> &constellation, unsigned msgq_limit, 
			      unsigned occupied_carriers, unsigned int fft_length);

/*!
 * \brief take a stream of bytes in and map to a vector of complex
 * constellation points suitable for IFFT input to be used in an ofdm
 * modulator.  Abstract class must be subclassed with specific mapping.
 * \ingroup modulation_blk
 * \ingroup ofdm_blk
 */

class DIGITAL_API digital_ofdm_mapper_bcv : public gr_sync_block
{
  friend DIGITAL_API digital_ofdm_mapper_bcv_sptr
  digital_make_ofdm_mapper_bcv (const std::vector<gr_complex> &constellation, unsigned msgq_limit, 
				unsigned occupied_carriers, unsigned int fft_length);
protected:
  digital_ofdm_mapper_bcv (const std::vector<gr_complex> &constellation, unsigned msgq_limit, 
			   unsigned occupied_carriers, unsigned int fft_length);

 private:
  std::vector<gr_complex> d_constellation;
  gr_msg_queue_sptr	d_msgq;
  gr_message_sptr	d_msg;
  unsigned		d_msg_offset;
  bool			d_eof;
  
  unsigned int 		d_occupied_carriers;
  unsigned int 		d_fft_length;
  unsigned int 		d_bit_offset;
  int			d_pending_flag;

  unsigned long  d_nbits;
  unsigned char  d_msgbytes;
  
  unsigned char d_resid;
  unsigned int d_nresid;

  std::vector<int> d_subcarrier_map;

  int randsym();

 public:
  ~digital_ofdm_mapper_bcv(void);

  gr_msg_queue_sptr	msgq() const { return d_msgq; }

  int work(int noutput_items,
	   gr_vector_const_void_star &input_items,
	   gr_vector_void_star &output_items);

};

#endif
