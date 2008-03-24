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

#ifndef INCLUDED_GC_JD_QUEUE_H
#define INCLUDED_GC_JD_QUEUE_H

#include "gc_jd_queue_data.h"

__GC_BEGIN_DECLS

/*!
 * \brief Initialize the queue to empty.
 */
void 
gc_jd_queue_init(gc_jd_queue_t *q);
  

/*!
 * \brief Add \p item to the tail of \p q.
 */
void 
gc_jd_queue_enqueue(gc_jd_queue_t *q, gc_job_desc_t *item);


/*!
 * \brief Remove and return item at head of queue, or 0 if queue is empty
 */
gc_job_desc_t *
gc_jd_queue_dequeue(gc_jd_queue_t *q);

__GC_END_DECLS


#endif /* INCLUDED_GC_JD_QUEUE_H */
