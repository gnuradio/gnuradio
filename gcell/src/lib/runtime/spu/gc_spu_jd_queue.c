/* -*- c++ -*- */
/*
 * Copyright 2007,2008 Free Software Foundation, Inc.
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
#include "gc_delay.h"
#include "gc_random.h"

#define MIN(a,b) ((a) < (b) ? (a) : (b))

extern int gc_sys_tag;

#define	INITIAL_BACKOFF	   32.0
#define MAX_BACKOFF	16384.0
#define	RANDOM_WEIGHT	    0.2

static float
next_backoff(float backoff)
{
  // exponential with random
  float t = backoff * 2.0;
  if (t > MAX_BACKOFF)
    t = MAX_BACKOFF;

  float r = (RANDOM_WEIGHT * (2.0 * (gc_uniform_deviate() - 0.5)));
  t = t * (1.0 + r);

  return t;
}

bool
gc_jd_queue_dequeue(gc_eaddr_t q, gc_eaddr_t *item_ea,
		    int jd_tag, gc_job_desc_t *item)
{
  int	status;
  char 	_tmp[256];
  gc_jd_queue_t *local_q =
    (gc_jd_queue_t *) ALIGN(_tmp, 128);		// get cache-aligned buffer
  
  float backoff = next_backoff(INITIAL_BACKOFF);

  do {
    // Copy the queue structure in and get a lock line reservation.
    // (The structure is 128-byte aligned and completely fills a cache-line)

    mfc_getllar(local_q, q, 0, 0);
    spu_readch(MFC_RdAtomicStat);

    if (local_q->mutex != 0)		// somebody else has it locked
      return false;

    if (local_q->head == 0)		// the queue is empty
      return false;

    // Try to acquire the lock

    local_q->mutex = 1;
    mfc_putllc(local_q, q, 0, 0);
    status = spu_readch(MFC_RdAtomicStat);

    if (status != 0){
      gc_cdelay((int) backoff);
      backoff = next_backoff(backoff);
    }

  } while (status != 0);

  // we're now holding the lock
    
  // copy in job descriptor at head of queue
  *item_ea = local_q->head;
  
  // We must use the fence with the jd_tag to ensure that any
  // previously initiated put of a job desc is locally ordered before
  // the get of the new one.
  mfc_getf(item, local_q->head, sizeof(gc_job_desc_t), jd_tag, 0, 0);
  mfc_write_tag_mask(1 << jd_tag);	// the tag we're interested in
  mfc_read_tag_status_all();		// wait for DMA to complete

  local_q->head = item->sys.next;
  item->sys.next = 0;
  if (local_q->head == 0)		// now empty?
    local_q->tail = 0;

  // Copy the queue struct back out and unlock the mutex in one fell swoop.
  // We use the unconditional put since it's faster and we own the lock.

  local_q->mutex = 0;
  mfc_putlluc(local_q, q, 0, 0);
  spu_readch(MFC_RdAtomicStat);

  return true;
}
