/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <mb_message.h>

mb_message_sptr
mb_make_message(pmt_t signal, pmt_t data, pmt_t metadata, mb_pri_t priority)
{
  return mb_message_sptr(new mb_message(signal, data, metadata, priority));
}

mb_message::mb_message(pmt_t signal, pmt_t data, pmt_t metadata, mb_pri_t priority)
  : d_signal(signal), d_data(data), d_metadata(metadata), d_priority(priority)
{
}

mb_message::~mb_message()
{
  // NOP
}
