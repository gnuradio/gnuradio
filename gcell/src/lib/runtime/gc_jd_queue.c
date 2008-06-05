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

#include "gc_jd_queue.h"
#include "memory_barrier.h"
#include <mutex_init.h>
#include <mutex_lock.h>
#include <mutex_unlock.h>

void 
gc_jd_queue_init(gc_jd_queue_t *q)
{
  _mutex_init(ptr_to_ea(&q->m.mutex));
  q->l.head = 0;
  q->l.tail = 0;
  q->f.flag = 0;
  smp_wmb();
}
  
void
gc_jd_queue_enqueue(gc_jd_queue_t *q, gc_job_desc_t *item)
{
  item->sys.next = 0;
  _mutex_lock(ptr_to_ea(&q->m.mutex));
  smp_rmb();		// import barrier

  if (q->l.tail == 0){    // currently empty
    q->l.tail = q->l.head = jdp_to_ea(item);
  }
  else {		// not empty, append
    ea_to_jdp(q->l.tail)->sys.next = jdp_to_ea(item);
    q->l.tail = jdp_to_ea(item);
  }

  smp_wmb();		// orders stores above before clearing of mutex
  _mutex_unlock(ptr_to_ea(&q->m.mutex));

  // let SPE's know we wrote something if they've got a lock-line reservation
  q->f.flag = 1;
}

gc_job_desc_t *
gc_jd_queue_dequeue(gc_jd_queue_t *q)
{
  _mutex_lock(ptr_to_ea(&q->m.mutex));
  smp_rmb();		// import barrier
  
  gc_eaddr_t item_ea = q->l.head;
  if (item_ea == 0){	// empty
    _mutex_unlock(ptr_to_ea(&q->m.mutex));
    return 0;
  }

  q->l.head = ea_to_jdp(item_ea)->sys.next;
  if (q->l.head == 0)	// now emtpy
    q->l.tail = 0;

  gc_job_desc_t *item = ea_to_jdp(item_ea);
  item->sys.next = 0;

  smp_wmb();		// orders stores above before clearing of mutex
  _mutex_unlock(ptr_to_ea(&q->m.mutex));
  return item;
}
