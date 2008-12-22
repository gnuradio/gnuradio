/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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
#ifndef INCLUDED_GCELL_GC_LOGGING_H
#define INCLUDED_GCELL_GC_LOGGING_H

#include <gcell/gc_types.h>
#include <string.h>

__GC_BEGIN_DECLS

typedef struct gc_log {
  gc_eaddr_t   	base;		// gc_log_entry_t * (16 byte aligned)
  uint32_t	nentries;	// number of entries (power-of-2)
} gc_log_t;

typedef struct gc_log_entry {
  uint32_t	seqno;		// monotonic sequence number
  uint32_t	timestamp;	// decrementer value (wraps every 53s on PS3)
  uint16_t	subsystem;	// 0 to 255 reserved for system, user gets 256 and up
  uint16_t	event;
  uint32_t	info[5];
} _AL16 gc_log_entry_t;

#define	GCL_SS_SYS	0	// lowest system reserved subsystem
#define	GCL_SS_USER   256	// lowest user reserved subsystem


/*
 * The resulting log files can be displayed using using:
 *
 *  $ od -t x4 -w32 spu_log.00 | less
 */


#if defined(__SPU__)

/*!
 * System fills in seqno and timestamp.  User is responsible for the rest.
 */

void _gc_log_write(gc_log_entry_t entry);

#ifdef ENABLE_GC_LOGGING
#define gc_log_write(entry) _gc_log_write(entry)
#else
#define gc_log_write(entry) do { } while (0)
#endif

inline static void
gc_log_write0(int subsystem, int event)
{
  gc_log_entry_t e;
  e.subsystem = subsystem;
  e.event = event;
  e.info[0] = 0;
  e.info[1] = 0;
  e.info[2] = 0;
  e.info[3] = 0;
  e.info[4] = 0;
  gc_log_write(e);
}

inline static void
gc_log_write1(int subsystem, int event,
	      uint32_t info0)
{
  gc_log_entry_t e;
  e.subsystem = subsystem;
  e.event = event;
  e.info[0] = info0;
  e.info[1] = 0;
  e.info[2] = 0;
  e.info[3] = 0;
  e.info[4] = 0;
  gc_log_write(e);
}

inline static void
gc_log_write2(int subsystem, int event,
	      uint32_t info0, uint32_t info1)
{
  gc_log_entry_t e;
  e.subsystem = subsystem;
  e.event = event;
  e.info[0] = info0;
  e.info[1] = info1;
  e.info[2] = 0;
  e.info[3] = 0;
  e.info[4] = 0;
  gc_log_write(e);
}

inline static void
gc_log_write3(int subsystem, int event,
	      uint32_t info0, uint32_t info1, uint32_t info2)
{
  gc_log_entry_t e;
  e.subsystem = subsystem;
  e.event = event;
  e.info[0] = info0;
  e.info[1] = info1;
  e.info[2] = info2;
  e.info[3] = 0;
  e.info[4] = 0;
  gc_log_write(e);
}

inline static void
gc_log_write4(int subsystem, int event,
	      uint32_t info0, uint32_t info1, uint32_t info2, uint32_t info3)
{
  gc_log_entry_t e;
  e.subsystem = subsystem;
  e.event = event;
  e.info[0] = info0;
  e.info[1] = info1;
  e.info[2] = info2;
  e.info[3] = info3;
  e.info[4] = 0;
  gc_log_write(e);
}

inline static void
gc_log_write5(int subsystem, int event,
	      uint32_t info0, uint32_t info1, uint32_t info2, uint32_t info3, uint32_t info4)
{
  gc_log_entry_t e;
  e.subsystem = subsystem;
  e.event = event;
  e.info[0] = info0;
  e.info[1] = info1;
  e.info[2] = info2;
  e.info[3] = info3;
  e.info[4] = info4;
  gc_log_write(e);
}

/*!
 * One time initialization called by system runtime
 */
void
_gc_log_init(gc_log_t log_info);

#endif

__GC_END_DECLS

#endif /* INCLUDED_GCELL_GC_LOGGING_H */
