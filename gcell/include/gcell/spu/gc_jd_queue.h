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

#ifndef INCLUDED_GCELL_SPU_GC_JD_QUEUE_H
#define INCLUDED_GCELL_SPU_GC_JD_QUEUE_H

#include <gcell/gc_jd_queue_data.h>

/*
 * Declarations for SPU side of job queue interface
 */

__GC_BEGIN_DECLS

/*!
 * \brief Remove and return item at head of queue.
 *
 * \param[in]  q is EA address of queue structure.
 * \param[out] item_ea is EA address of item at head of queue.
 * \param[in]  jd_tag is the tag to use to get the LS copy of the item.
 * \param[out] item is local store copy of item at head of queue.
 * \returns false if the queue is empty, otherwise returns true
 *   and sets \p item_ea and DMA's job descriptor into \p item
 *
 * If return is false, we're holding a lock-line reservation that
 * covers the queue.
 */
bool
gc_jd_queue_dequeue(gc_eaddr_t q, gc_eaddr_t *item_ea,
		    int jd_tag, gc_job_desc_t *item);

__GC_END_DECLS


#endif /* INCLUDED_GCELL_SPU_GC_JD_QUEUE_H */
