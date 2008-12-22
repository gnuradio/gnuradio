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

#ifndef INCLUDED_GCELL_GC_JD_STACK_H
#define INCLUDED_GCELL_GC_JD_STACK_H

#include <gcell/gc_types.h>
#include <gcell/gc_job_desc.h>

__GC_BEGIN_DECLS

/*!
 * \brief Lock free stack for job descriptors (used for free list)
 *
 * This is aligned to a cache line, and fills the cache line,
 * to avoid inadvertently losing reservations created with
 * the load-and-reserve instructions.
 */

typedef struct gc_jd_stack
{
  gc_eaddr_t	top;

  // pad out to a full cache line
  uint8_t	_pad[128 - sizeof(gc_eaddr_t)];
} _AL128 gc_jd_stack_t;


/*!
 * \brief Initialize the stack to empty.
 */
void 
gc_jd_stack_init(gc_jd_stack_t *stack);
  

/*!
 * \brief Add \p item to the top of \p stack.
 */
void 
gc_jd_stack_push(gc_jd_stack_t *stack, gc_job_desc_t *item);


/*!
 * \brief pop and return top item on stack, or 0 if stack is empty
 */
gc_job_desc_t *
gc_jd_stack_pop(gc_jd_stack_t *stack);

__GC_END_DECLS


#endif /* INCLUDED_GCELL_GC_JD_STACK_H */
