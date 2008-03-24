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

#ifndef INCLUDED_GC_JD_QUEUE_DATA_H
#define INCLUDED_GC_JD_QUEUE_DATA_H

#include "gc_types.h"
#include "gc_job_desc.h"

__GC_BEGIN_DECLS

/*!
 * \brief (Lock free someday...) queue for job descriptors
 *
 * This is the main data structure shared between PPEs and SPEs.
 * It is used to enqueue work for SPEs.  SPEs or PPEs may enqueue
 * work.  SPE's dequeue from here.
 *
 * FIXME make it lock free ;)  For now, use a spin lock.
 */
typedef struct gc_jd_queue
{
  gc_eaddr_t	head _AL16;
  gc_eaddr_t	tail _AL16;
  uint32_t	mutex;		// libsync mutex (spin lock)
} gc_jd_queue_t;


__GC_END_DECLS

#endif /* INCLUDED_GC_JD_QUEUE_DATA_H */


