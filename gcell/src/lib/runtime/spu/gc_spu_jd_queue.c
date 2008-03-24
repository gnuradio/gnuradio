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

#include "gc_jd_queue.h"
#include "mutex_lock.h"
#include "mutex_unlock.h"

extern int gc_sys_tag;

bool
gc_jd_queue_dequeue(gc_eaddr_t q, gc_eaddr_t *item_ea, gc_job_desc_t *item)
{
  gc_jd_queue_t	local_q;

  // Before aquiring the lock, see if it's possible that there's
  // something in the queue.  Checking in this way makes it easier
  // for the PPE to insert things, since we're not contending for
  // the lock unless there is something in the queue.

  // copy in the queue structure
  mfc_get(&local_q, q, sizeof(gc_jd_queue_t), gc_sys_tag, 0, 0);
  mfc_write_tag_mask(1 << gc_sys_tag);	// the tag we're interested in
  mfc_read_tag_status_all();		// wait for DMA to complete

  if (local_q.head == 0){		// empty
    return false;
  }

  // When we peeked, head was non-zero.  Now grab the
  // lock and do it for real.

  _mutex_lock(q + offsetof(gc_jd_queue_t, mutex));

  // copy in the queue structure
  mfc_get(&local_q, q, sizeof(gc_jd_queue_t), gc_sys_tag, 0, 0);
  mfc_write_tag_mask(1 << gc_sys_tag);	// the tag we're interested in
  mfc_read_tag_status_all();		// wait for DMA to complete

  if (local_q.head == 0){		// empty
    _mutex_unlock(q + offsetof(gc_jd_queue_t, mutex));
    return false;
  }

  // copy in job descriptor at head of queue
  *item_ea = local_q.head;
  mfc_get(item, local_q.head, sizeof(gc_job_desc_t), gc_sys_tag, 0, 0);
  mfc_write_tag_mask(1 << gc_sys_tag);	// the tag we're interested in
  mfc_read_tag_status_all();		// wait for DMA to complete

  local_q.head = item->sys.next;
  item->sys.next = 0;
  if (local_q.head == 0)		// now empty?
    local_q.tail = 0;


  // copy the queue structure back out
  mfc_put(&local_q, q, sizeof(gc_jd_queue_t), gc_sys_tag, 0, 0);
  mfc_write_tag_mask(1 << gc_sys_tag);	// the tag we're interested in
  mfc_read_tag_status_all();		// wait for DMA to complete

  // Q: FIXME do we need to order stores in EA or can we just clear the
  // local copy of the mutex above and blast it out, removing the need
  // for this explicit unlock?
  //
  // A: Manual says it's better to use an atomic op rather than
  // a normal DMA, and that a putlluc is better than a putllc if
  // you can use it.

  _mutex_unlock(q + offsetof(gc_jd_queue_t, mutex));
  return true;
}


void
gc_jd_queue_getllar(gc_eaddr_t q)
{
  // get reservation that includes the tail of the queue
  gc_eaddr_t	tail = q + offsetof(gc_jd_queue_t, tail);
    
  char _tmp[256];
  char *buf = (char *) ALIGN(_tmp, 128);	// get cache-aligned buffer

  mfc_getllar(buf, ALIGN128_EA(tail), 0, 0);
  spu_readch(MFC_RdAtomicStat);
}
