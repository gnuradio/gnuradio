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

// @WARNING@

#ifndef @GUARD_NAME@
#define @GUARD_NAME@

#include <gr_core_api.h>
#include <gr_sync_interpolator.h>

class @NAME@;
typedef boost::shared_ptr<@NAME@> @SPTR_NAME@;

GR_CORE_API @SPTR_NAME@ gr_make_@BASE_NAME@ (const std::vector<@O_TYPE@> &symbol_table, const int D = 1);

/*!
 * \brief Map a stream of symbol indexes (unpacked bytes or shorts) to stream of float or complex onstellation points.in \p D dimensions (\p D = 1 by default)
 * \ingroup converter_blk
 *
 * input: stream of @I_TYPE@; output: stream of @O_TYPE@
 *
 * out[n D + k] = symbol_table[in[n] D + k], k=0,1,...,D-1
 *
 * The combination of gr_packed_to_unpacked_XX followed by
 * gr_chunks_to_symbols_XY handles the general case of mapping 
 * from a stream of bytes or shorts into arbitrary float
 * or complex symbols.
 *
 * \sa gr_packed_to_unpacked_bb, gr_unpacked_to_packed_bb,
 * \sa gr_packed_to_unpacked_ss, gr_unpacked_to_packed_ss,
 * \sa gr_chunks_to_symbols_bf, gr_chunks_to_symbols_bc.
 * \sa gr_chunks_to_symbols_sf, gr_chunks_to_symbols_sc.
 */

class GR_CORE_API @NAME@ : public gr_sync_interpolator
{
  friend GR_CORE_API @SPTR_NAME@ gr_make_@BASE_NAME@ (const std::vector<@O_TYPE@> &symbol_table, const int D);

  int d_D;
  std::vector<@O_TYPE@> d_symbol_table;
  @NAME@ (const std::vector<@O_TYPE@> &symbol_table, const int D = 1);

 public:
  int D () const { return d_D; }
  std::vector<@O_TYPE@> symbol_table () const { return d_symbol_table; }
  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);

  bool check_topology(int ninputs, int noutputs) { return ninputs == noutputs; }
};

#endif
