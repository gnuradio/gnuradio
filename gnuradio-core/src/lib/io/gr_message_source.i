/* -*- c++ -*- */
/*
 * Copyright 2005 Free Software Foundation, Inc.
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

GR_SWIG_BLOCK_MAGIC(gr,message_source);

#ifdef SWIGGUILE
// Rename these.  Without this, the primitive bindings are OK, but the
// goops bindings try to create a bogus generic-function...
// See core.scm for the second part of the workaround.
%rename(message_source_limit_ctor) gr_make_message_source(size_t itemsize, int msgq_limit);
%rename(message_source_msgq_ctor)  gr_make_message_source(size_t itemsize, gr_msg_queue_sptr msgq);
#endif

gr_message_source_sptr gr_make_message_source (size_t itemsize, int msgq_limit=0);
gr_message_source_sptr gr_make_message_source (size_t itemsize, gr_msg_queue_sptr msgq);

class gr_message_source : public gr_sync_block
{
 protected:
  gr_message_source (size_t itemsize, int msgq_limit);
  gr_message_source (size_t itemsize, gr_msg_queue_sptr msgq);

 public:
  ~gr_message_source ();

  gr_msg_queue_sptr msgq() const;
};
