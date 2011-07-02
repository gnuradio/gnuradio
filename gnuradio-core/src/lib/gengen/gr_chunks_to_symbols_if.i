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

// WARNING: this file is machine generated.  Edits will be over written

GR_SWIG_BLOCK_MAGIC(gr,chunks_to_symbols_if);

gr_chunks_to_symbols_if_sptr gr_make_chunks_to_symbols_if (const std::vector<float> &symbol_table, const int D = 1);

class gr_chunks_to_symbols_if : public gr_sync_interpolator
{
private:
  gr_chunks_to_symbols_if (const std::vector<float> &symbol_table, const int D = 1);

public:
  int D () const { return d_D; }
  std::vector<float> symbol_table () const { return d_symbol_table; }
};
