/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
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

#ifndef INCLUDED_GR_MESSAGE_VECTOR_SOURCE_H
#define INCLUDED_GR_MESSAGE_VECTOR_SOURCE_H

#include <gr_sync_block.h>
#include <gr_message.h>
#include <gr_msg_queue.h>
#include <gr_frame.h>

class gr_message_vector_source;
typedef boost::shared_ptr<gr_message_vector_source> gr_message_vector_source_sptr;

gr_message_vector_source_sptr gr_make_message_vector_source (size_t max_msg_size, int msgq_limit=0);

/*!
 * \brief Turn received messages into vectors
 * \ingroup source
 */
class gr_message_vector_source : public gr_sync_block
{
 private:
  size_t	 	d_max_msg_size;
  gr_msg_queue_sptr	d_msgq;
  bool			d_eof;

  friend gr_message_vector_source_sptr
  gr_make_message_vector_source(size_t max_msg_size, int msgq_limit);

 protected:
  gr_message_vector_source (size_t max_msg_size, int msgq_limit);

 public:
  ~gr_message_vector_source ();

  gr_msg_queue_sptr	msgq() const { return d_msgq; }

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif /* INCLUDED_GR_MESSAGE_VECTOR_SOURCE_H */
