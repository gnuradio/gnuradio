/* -*- c++ -*- */
/*
 * Copyright 2002 Free Software Foundation, Inc.
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

#ifndef _ASSEMBLY_H_
#define _ASSEMBLY_H_

#if defined (__APPLE__) && defined (__APPLE_CC__)

// XCode ignores the .scl and .type functions in XCode 2.2.1 and 2.3,
// but creates an error in XCode 2.4.  Just ignore them.

#define GLOB_SYMB(f)    _ ## f

#define DEF_FUNC_HEAD(f)  /* none */

#define FUNC_TAIL(f)    /* none*/

#elif !defined (__ELF__)

/*
 * Too bad, the following define does not work as expected --SF
 * 	#define GLOB_SYMB(f)	__USER_LABEL_PREFIX__ ## f
 */
#define GLOB_SYMB(f)	_ ## f

#define DEF_FUNC_HEAD(f)	\
	.def	GLOB_SYMB(f); .scl 2; .type 32; .endef

#define FUNC_TAIL(f)	/* none */


#else	/* !__ELF__ */


#define GLOB_SYMB(f)	f

#define DEF_FUNC_HEAD(f)	\
	.type	GLOB_SYMB(f),@function	\

#define FUNC_TAIL(f)	\
  .Lfe1:		\
	.size	GLOB_SYMB(f),.Lfe1-GLOB_SYMB(f)


#endif	/* !__ELF__ */


#endif /* _ASSEMBLY_H_ */
