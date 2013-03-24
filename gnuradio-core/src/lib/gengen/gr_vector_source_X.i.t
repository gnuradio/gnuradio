/* -*- c++ -*- */
/*
 * Copyright 2004,2008,2012 Free Software Foundation, Inc.
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

@NAME@_sptr
gr_make_@BASE_NAME@ (const std::vector<@TYPE@> &data, bool repeat = false, int vlen = 1, const std::vector<gr_tag_t> &tags=std::vector<gr_tag_t>())
  throw(std::invalid_argument);

class @NAME@ : public gr_sync_block {
 public:
  void rewind();
  void set_data(const std::vector<@TYPE@> &data, const std::vector<gr_tag_t> &tags = std::vector<gr_tag_t>());
 private:
  @NAME@ (const std::vector<@TYPE@> &data, bool repeat, int vlen, const std::vector<gr_tag_t> &tags);
};
