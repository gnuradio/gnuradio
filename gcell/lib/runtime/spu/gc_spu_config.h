/* -*- c -*- */
/*
 * Copyright 2008,2009 Free Software Foundation, Inc.
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
#ifndef INCLUDED_GCELL_GC_SPU_CONFIG_H
#define INCLUDED_GCELL_GC_SPU_CONFIG_H

#include <gcell/gc_job_desc.h>

#define CACHE_LINE_SIZE	     128	      // in bytes

#if 1
# define	GC_SPU_BUFSIZE_BASE  (40 * 1024)      //  must be multiple of CACHE_LINE_SIZE
#else
# define	GC_SPU_BUFSIZE_BASE  (20 * 1024)      //  must be multiple of CACHE_LINE_SIZE
#endif

#define	GC_SPU_BUFSIZE (GC_SPU_BUFSIZE_BASE + MAX_ARGS_EA * CACHE_LINE_SIZE)

#define NGETBUFS	1	// gets are single buffered
#define NPUTBUFS	2	// puts are double buffered

#endif /* INCLUDED_GCELL_GC_SPU_CONFIG_H */
