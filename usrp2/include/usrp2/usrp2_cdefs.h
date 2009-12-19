/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef INCLUDED_USRP2_CDEFS_H
#define INCLUDED_USRP2_CDEFS_H

/* C++ needs to know that types and declarations are C, not C++.  */
#ifdef	__cplusplus
# define __U2_BEGIN_DECLS	extern "C" {
# define __U2_END_DECLS	}
#else
# define __U2_BEGIN_DECLS
# define __U2_END_DECLS
#endif

#endif /* INCLUDED_USRP2_CDEFS_H */
