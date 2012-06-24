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

#include <gr_shared_block_sptr.h>
#include <gr_vector_source_i.h>

gr_block_sptr
foo (gr_vector_source_i_sptr s)
{
  return gr_block_sptr (s);
}

typedef gr_shared_block_sptr<gr_vector_source_i> gr_vector_source_i_ptrX;
//typedef boost::shared_ptr<gr_vector_source_i> gr_vector_source_i_ptrX;

gr_vector_source_i_ptrX
bar (gr_vector_source_i *s)
{
  return gr_vector_source_i_ptrX (s);
}

gr_block_sptr
baz_1 (gr_vector_source_i_ptrX s)
{
  return gr_block_sptr (s);
}

#if 0
gr_block_sptr
baz_2 (gr_vector_source_i_ptrX s)
{
  return s.block_sptr ();
}
#endif
