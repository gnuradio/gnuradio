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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_message_vector_source.h>
#include <gr_io_signature.h>
#include <cstdio>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdexcept>


// public constructor that returns a shared_ptr

gr_message_vector_source_sptr
gr_make_message_vector_source(size_t max_msg_size, int msgq_limit)
{
  return gr_message_vector_source_sptr(new gr_message_vector_source(max_msg_size,msgq_limit));
}

gr_message_vector_source::gr_message_vector_source (size_t max_msg_size, int msgq_limit)
  : gr_sync_block("message_vector_source",
		  gr_make_io_signature(0, 0, 0),
		  gr_make_io_signature(1, 1, 2*sizeof(int) + max_msg_size * sizeof(char))), // Make room for length fields
    d_max_msg_size(max_msg_size), d_msgq(gr_make_msg_queue(msgq_limit)), d_eof(false)
{
}

gr_message_vector_source::~gr_message_vector_source()
{
}

int
gr_message_vector_source::work(int noutput_items,
			       gr_vector_const_void_star &input_items,
			       gr_vector_void_star &output_items)
{
  /*
  char *out = (char *) output_items[0];

  if (d_eof)
      return -1;

  gr_message_sptr msg = d_msgq->delete_head();
  
  if (msg->type() == 1) {	           // type == 1 sets EOF
    d_eof = true;
  }

  assert(msg->length() <= d_max_msg_size);
  memset((int*)out, msg->length(), 1);
  memcpy (&out[4], (msg->msg()), msg->length());
  */

  char *out = (char *) output_items[0];
  gr_frame *myframe = (gr_frame*)out;

  if (d_eof)
      return -1;

  gr_message_sptr msg = d_msgq->delete_head();
  
  if (msg->type() == 1) {	           // type == 1 sets EOF
    d_eof = true;
  }

  assert(msg->length() <= d_max_msg_size);
  myframe->length = msg->length();
  myframe->mtu = d_max_msg_size;
  memcpy (myframe->data, (msg->msg()), msg->length());

  return 1;
}
