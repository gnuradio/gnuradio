/* -*- c++ -*- */
/*
 * Copyright 2009 Free Software Foundation, Inc.
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

%include "gnuradio.i"			// the common stuff

// We'll need something like this if we add C++ blocks...
// See the gr-howto-write-a-block tarball for examples

// ----------------------------------------------------------------

/*
 * First arg is the package prefix.
 * Second arg is the name of the class minus the prefix.
 *
 * This does some behind-the-scenes magic so we can
 * access ra_square_ff from python as ra.square_ff
 */
// GR_SWIG_BLOCK_MAGIC(ra,square_ff);

// ra_square_ff_sptr ra_make_square_ff ();
// 
// class ra_square_ff : public gr_block
// {
// private:
//   ra_square_ff ();
// };
// 
// // ----------------------------------------------------------------
// 
// GR_SWIG_BLOCK_MAGIC(ra,square2_ff);
// 
// ra_square2_ff_sptr ra_make_square2_ff ();
// 
// class ra_square2_ff : public gr_sync_block
// {
// private:
//   ra_square2_ff ();
// };
