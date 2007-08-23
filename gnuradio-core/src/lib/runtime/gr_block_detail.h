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
 * GNU General Public License for more detail.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_GR_BLOCK_DETAIL_H
#define INCLUDED_GR_BLOCK_DETAIL_H

#include <gr_runtime_types.h>
#include <stdexcept>

/*!
 * \brief Implementation details to support the signal processing abstraction
 * \ingroup internal
 *
 * This class contains implementation detail that should be "out of sight"
 * of almost all users of GNU Radio.  This decoupling also means that
 * we can make changes to the guts without having to recompile everything.
 */

class gr_block_detail {
 public:
  ~gr_block_detail ();

  int ninputs () const { return d_ninputs; }
  int noutputs () const { return d_noutputs; }
  bool sink_p () const { return d_noutputs == 0; }
  bool source_p () const { return d_ninputs == 0; }

  void set_done (bool done);
  bool done () const { return d_done; }

  void set_input (unsigned int which, gr_buffer_reader_sptr reader);
  gr_buffer_reader_sptr input (unsigned int which)
  {
    if (which >= d_ninputs)
      throw std::invalid_argument ("gr_block_detail::input");
    return d_input[which];
  }

  void set_output (unsigned int which, gr_buffer_sptr buffer);
  gr_buffer_sptr output (unsigned int which)
  {
    if (which >= d_noutputs)
      throw std::invalid_argument ("gr_block_detail::output");
    return d_output[which];
  }

  /*!
   * \brief Tell the scheduler \p how_many_items of input stream \p which_input were consumed.
   */
  void consume (int which_input, int how_many_items);

  /*!
   * \brief Tell the scheduler \p how_many_items were consumed on each input stream.
   */
  void consume_each (int how_many_items);

  void produce_each (int how_many_items);

  // ----------------------------------------------------------------------------

 private:
  unsigned int                       d_ninputs;
  unsigned int                       d_noutputs;
  std::vector<gr_buffer_reader_sptr> d_input;
  std::vector<gr_buffer_sptr>	     d_output;
  bool                               d_done;

  gr_block_detail (unsigned int ninputs, unsigned int noutputs);

  friend gr_block_detail_sptr
  gr_make_block_detail (unsigned int ninputs, unsigned int noutputs);
};

gr_block_detail_sptr
gr_make_block_detail (unsigned int ninputs, unsigned int noutputs);

long
gr_block_detail_ncurrently_allocated ();

#endif /* INCLUDED_GR_BLOCK_DETAIL_H */
