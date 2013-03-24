/* -*- c++ -*- */
/*
 * Copyright 2004,2008,2009 Free Software Foundation, Inc.
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

// @WARNING@


GR_SWIG_BLOCK_MAGIC(gr,@BASE_NAME@);

@SPTR_NAME@ gr_make_@BASE_NAME@ (int vlen = 1);

class @NAME@ : public gr_sync_block {
 private:
  @NAME@ (int vlen);

 public:
  void clear();			// deprecated
  void reset();
  std::vector<@TYPE@> data () const;
  std::vector<gr_tag_t> tags () const;
};

