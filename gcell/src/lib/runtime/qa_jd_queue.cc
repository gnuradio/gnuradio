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

#include "qa_jd_queue.h"
#include <cppunit/TestAssert.h>
#include "gc_jd_queue.h"
#include <stdio.h>



static const int NJDS = 16;
static gc_jd_queue_t queue;
static gc_job_desc_t jds[NJDS];

// no brainer, single threaded basic checkout
void
qa_jd_queue::t1()
{
  // N.B., queue allocated stuff doesn't obey ((aligned (N))) attributes
  //const int NJDS = 8;
  //gc_jd_queue_t queue;
  //gc_job_desc_t jds[NJDS];

  //printf("&queue   = %p\n", &queue);
  //printf("&jds[0] = %p\n", &jds[0]);
  //printf("&jds[1] = %p\n", &jds[1]);

  CPPUNIT_ASSERT(((uintptr_t) &queue & 0x7f) == 0);
  CPPUNIT_ASSERT(((uintptr_t) &jds[0] & 0x7f) == 0);
  CPPUNIT_ASSERT(((uintptr_t) &jds[1] & 0x7f) == 0);

  gc_jd_queue_init(&queue);

  CPPUNIT_ASSERT(gc_jd_queue_dequeue(&queue) == 0);

  gc_jd_queue_enqueue(&queue, &jds[0]);
  CPPUNIT_ASSERT_EQUAL(&jds[0], gc_jd_queue_dequeue(&queue));

  CPPUNIT_ASSERT(gc_jd_queue_dequeue(&queue) == 0);

  for (int i = 0; i < NJDS; i++)
    gc_jd_queue_enqueue(&queue, &jds[i]);

  for (int i = 0; i < NJDS; i++)
    CPPUNIT_ASSERT_EQUAL(&jds[i], gc_jd_queue_dequeue(&queue));

  CPPUNIT_ASSERT(gc_jd_queue_dequeue(&queue) == 0);
}

// FIXME multithreaded (running on PPE)
void
qa_jd_queue::t2()
{
}

// FIXME multithreaded (running on PPE & SPE)
void
qa_jd_queue::t3()
{
}
