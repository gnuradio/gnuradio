/* -*- c++ -*- */
/*
 * Copyright 2010 Free Software Foundation, Inc.
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

GR_SWIG_BLOCK_MAGIC(gr,annotator_1toall);

gr_annotator_1toall_sptr gr_make_annotator_1toall (size_t sizeof_stream_item, float rel_rate);

class gr_annotator_1toall : public gr_block
{
public:
  void set_rel_rate(float rrate) { d_rel_rate = rrate; set_relative_rate(d_rel_rate); }
  float rel_rate() { return d_rel_rate; }
  std::vector<pmt::pmt_t> data() const;
  
private:
  gr_annotator_1toall (size_t sizeof_stream_item, float rel_rate);
};

