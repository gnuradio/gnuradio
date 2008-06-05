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

/*
 * ea must be 128-byte aligned, the mutex is in the first int32_t, and
 * it must be safe to write the remaining 124 bytes with anything at
 * all.
 */
static __inline void _fast_mutex_unlock(mutex_ea_t ea)
{
  char _tmp[256];
  vector signed int *buf
    = (vector signed int *) ALIGN(_tmp, 128);	// get cache-aligned buffer

  buf[0] = spu_splats(0);	// the value that unlocks the mutex

  mfc_putlluc(buf, ea, 0, 0);	// unconditional put, no reservation reqd
  spu_readch(MFC_RdAtomicStat);
}



bool
gc_jd_queue_dequeue(gc_eaddr_t q, gc_eaddr_t *item_ea,
		    int jd_tag, gc_job_desc_t *item)
{
  gc_jd_q_links_t	local_q;

  // Before aquiring the lock, see if it's possible that there's
  // something in the queue.  Checking in this way makes it easier
  // for the PPE to insert things, since we're not contending for
  // the lock unless there is something in the queue.

  // copy in the queue structure
  mfc_get(&local_q, q, sizeof(local_q), gc_sys_tag, 0, 0);
  mfc_write_tag_mask(1 << gc_sys_tag);	// the tag we're interested in
  mfc_read_tag_status_all();		// wait for DMA to complete

  if (local_q.head == 0){		// empty
    return false;
  }

  // When we peeked, head was non-zero.  Now grab the
  // lock and do it for real.

  _mutex_lock(q + offsetof(gc_jd_queue_t, m.mutex));

  // copy in the queue structure
  mfc_get(&local_q, q, sizeof(local_q), gc_sys_tag, 0, 0);
  mfc_write_tag_mask(1 << gc_sys_tag);	// the tag we're interested in
  mfc_read_tag_status_all();		// wait for DMA to complete

  if (local_q.head == 0){		// empty
    _fast_mutex_unlock(q + offsetof(gc_jd_queue_t, m.mutex));
    return false;
  }

  // copy in job descriptor at head of queue
  *item_ea = local_q.head;
  
  // We must use the fence with the jd_tag to ensure that any
  // previously initiated put of a job desc is locally ordered before
  // the get of the new one.
  mfc_getf(item, local_q.head, sizeof(gc_job_desc_t), jd_tag, 0, 0);
  mfc_write_tag_mask(1 << jd_tag);	// the tag we're interested in
  mfc_read_tag_status_all();		// wait for DMA to complete

  local_q.head = item->sys.next;
  item->sys.next = 0;
  if (local_q.head == 0)		// now empty?
    local_q.tail = 0;


  // copy the queue structure back out
  mfc_put(&local_q, q, sizeof(local_q), gc_sys_tag, 0, 0);
  mfc_write_tag_mask(1 << gc_sys_tag);	// the tag we're interested in
  mfc_read_tag_status_all();		// wait for DMA to complete

  // Q: FIXME do we need to order stores in EA or can we just clear the
  // local copy of the mutex above and blast it out, removing the need
  // for this explicit unlock?
  //
  // A: Manual says it's better to use an atomic op rather than
  // a normal DMA, and that a putlluc is better than a putllc if
  // you can use it.

  _fast_mutex_unlock(q + offsetof(gc_jd_queue_t, m.mutex));
  return true;
}


void
gc_jd_queue_getllar(gc_eaddr_t q)
{
  // get reservation that includes the flag in the queue
  gc_eaddr_t	ea = q + offsetof(gc_jd_queue_t, f.flag);
    
  char _tmp[256];
  char *buf = (char *) ALIGN(_tmp, 128);	// get cache-aligned buffer

  mfc_getllar(buf, ALIGN128_EA(ea), 0, 0);
  spu_readch(MFC_RdAtomicStat);
}
