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

#ifndef INCLUDED_GNURADIO_SWIG_BUG_WORKAROUND_H
#define INCLUDED_GNURADIO_SWIG_BUG_WORKAROUND_H

/*
 * This include files works around a bug in SWIG 1.3.21 and 22
 * where it fails to emit these declarations when doing
 * %import "gnuradio.i"
 */

class gr_base_error_handler;
class gr_basic_block;
class gr_block;
class gr_error_handler;
class gr_file_error_handler;
class gr_hier_block2;
class gr_msg_handler;
class gr_msg_queue;
class gr_sync_block;
class gr_sync_decimator;
class gr_sync_interpolator;
class gr_top_block;

#endif /* INCLUDED_GNURADIO_SWIG_BUG_WORKAROUND_H */
