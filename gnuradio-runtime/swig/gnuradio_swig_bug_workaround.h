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

class base_error_handler;
class basic_block;
class block;
class file_error_handler;
class hier_block2;
class msg_handler;
class msg_queue;
class sync_block;
class sync_decimator;
class sync_interpolator;
class top_block;

#endif /* INCLUDED_GNURADIO_SWIG_BUG_WORKAROUND_H */
