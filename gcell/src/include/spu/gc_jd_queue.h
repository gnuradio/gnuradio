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

#ifndef INCLUDED_SPU_GC_JD_QUEUE_H
#define INCLUDED_SPU_GC_JD_QUEUE_H

#include "gc_jd_queue_data.h"

/*
 * Declarations for SPU side of job queue interface
 */

__GC_BEGIN_DECLS

/*!
 * \brief Remove and return item at head of queue.
 *
 * \param[in]  q is EA address of queue structure.
 * \param[out] item_ea is EA address of item at head of queue.
 * \param[out] item is local store copy of item at head of queue.
 * \returns false if the queue is empty, otherwise returns true
 *   and sets \p item_ea and DMA's job descriptor into \p item
 */
bool
gc_jd_queue_dequeue(gc_eaddr_t q, gc_eaddr_t *item_ea, gc_job_desc_t *item);


/*!
 * \brief Get a line reservation on the queue
 *
 * \param[in]  q is EA address of queue structure.
 */
void
gc_jd_queue_getllar(gc_eaddr_t q);

__GC_END_DECLS


#endif /* INCLUDED_SPU_GC_JD_QUEUE_H */
