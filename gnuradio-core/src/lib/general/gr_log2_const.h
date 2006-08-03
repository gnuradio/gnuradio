/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */


/*
 * a bit of template hackery...
 */
#ifndef INCLUDED_GR_LOG2_CONST_H
#define INCLUDED_GR_LOG2_CONST_H

#include <assert.h>

template<unsigned int k> static inline int gr_log2_const() { assert(0); }

template<> static inline int gr_log2_const<1>()   { return 0; }
template<> static inline int gr_log2_const<2>()   { return 1; }
template<> static inline int gr_log2_const<4>()   { return 2; }
template<> static inline int gr_log2_const<8>()   { return 3; }
template<> static inline int gr_log2_const<16>()  { return 4; }
template<> static inline int gr_log2_const<32>()  { return 5; }
template<> static inline int gr_log2_const<64>()  { return 6; }
template<> static inline int gr_log2_const<128>() { return 7; }
template<> static inline int gr_log2_const<256>() { return 8; }
template<> static inline int gr_log2_const<512>() { return 9; }
template<> static inline int gr_log2_const<1024>(){ return 10; }

#endif /* INCLUDED_GR_LOG2_CONST_H */
