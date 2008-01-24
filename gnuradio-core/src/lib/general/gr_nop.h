/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_NOP_H
#define INCLUDED_GR_NOP_H

#include <gr_block.h>
#include <stddef.h>			// size_t

/*!
 * \brief Does nothing.  Used for testing only.
 * \ingroup misc
 */
class gr_nop : public gr_block
{
  friend gr_block_sptr gr_make_nop (size_t sizeof_stream_item);

  gr_nop (size_t sizeof_stream_item);

 public:
  virtual int general_work (int noutput_items,
			    gr_vector_int &ninput_items,
			    gr_vector_const_void_star &input_items,
			    gr_vector_void_star &output_items);
};

gr_block_sptr
gr_make_nop (size_t sizeof_stream_item);

#endif /* INCLUDED_GR_NOP_H */
