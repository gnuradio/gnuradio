/* -*- c -*- */
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

#ifndef INCLUDED_GC_JOB_DESC_PRIVATE_H
#define INCLUDED_GC_JOB_DESC_PRIVATE_H

// #include <libsync.h>

/*!
 * \brief Implementation details we'd like to hide from the user.
 */
typedef struct gc_job_desc_private
{
  gc_eaddr_t	next;		    // used to implement job queue and free list
  uint16_t	job_id;
  uint16_t	client_id;
  uint32_t	direction_union;    // union of all gc_job_ea_arg.direction fields
} gc_job_desc_private_t;

#endif /* INCLUDED_GC_JOB_PRIVATE_H */

