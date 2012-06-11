/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_KEEP_M_IN_N_H
#define INCLUDED_GR_KEEP_M_IN_N_H

#include <gr_core_api.h>
#include <gr_block.h>

class gr_keep_m_in_n;
typedef boost::shared_ptr<gr_keep_m_in_n> gr_keep_m_in_n_sptr;

GR_CORE_API gr_keep_m_in_n_sptr
gr_make_keep_m_in_n (size_t item_size, int m, int n, int offset);


/*!
 * \brief decimate a stream, keeping one item out of every n.
 * \ingroup slicedice_blk
 */
class GR_CORE_API gr_keep_m_in_n : public gr_block
{
  friend GR_CORE_API gr_keep_m_in_n_sptr
  gr_make_keep_m_in_n (size_t item_size, int m, int n, int offset);

  int	d_n;
  int	d_m;
  int	d_count;
  int   d_offset;
  int   d_itemsize;

 protected:
  gr_keep_m_in_n (size_t item_size, int m, int n, int offset);
  void forecast (int noutput_items, gr_vector_int &ninput_items_required);

 public:
  int general_work (int noutput_items,
            gr_vector_int &ninput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items);

    void set_offset(int offset);
    void set_n(int n){ d_n = n; }
    void set_m(int m){ d_m = m; }

};

#endif /* INCLUDED_GR_KEEP_M_IN_N_H */
