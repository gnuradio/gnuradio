/* -*- c++ -*- */
/*
 * Copyright 2005 Free Software Foundation, Inc.
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

%ignore gr_skiphead;
class gr_skiphead : public gr_block {
  friend gr_block_sptr gr_make_skiphead (size_t sizeof_stream_item, int nitems);
  gr_skiphead (size_t sizeof_stream_item, int nitems);
};

%rename(skiphead) gr_make_skiphead;
gr_block_sptr gr_make_skiphead (size_t sizeof_stream_item, int nitems);
