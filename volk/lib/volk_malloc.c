/* -*- c -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include <volk/volk_malloc.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * For #defines used to determine support for allocation functions,
 * see: http://linux.die.net/man/3/aligned_alloc
*/

// Disabling use of aligned_alloc. This function requires that size be
// a multiple of alignment, which is too restrictive for many uses of
// VOLK.

//// If we are using C11 standard, use the aligned_alloc
//#ifdef _ISOC11_SOURCE
//
//void *volk_malloc(size_t size, size_t alignment)
//{
//  void *ptr = aligned_alloc(alignment, size);
//  if(ptr == NULL) {
//    fprintf(stderr, "VOLK: Error allocating memory (aligned_alloc)\n");
//  }
//  return ptr;
//}
//
//void volk_free(void *ptr)
//{
//  free(ptr);
//}
//
//#else // _ISOC11_SOURCE

// Otherwise, test if we are a POSIX or X/Open system
// This only has a restriction that alignment be a power of 2.
#if _POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600

void *volk_malloc(size_t size, size_t alignment)
{
  void *ptr;
  int err = posix_memalign(&ptr, alignment, size);
  if(err == 0) {
    return ptr;
  }
  else {
    fprintf(stderr, "VOLK: Error allocating memory (posix_memalign: %d)\n", err);
    return NULL;
  }
}

void volk_free(void *ptr)
{
  free(ptr);
}

// No standard handlers; we'll do it ourselves.
#else // _POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600

typedef struct mbuf_t {
  void *orig;
  void *align;
  struct mbuf_t *next;
} mbuf;

/*
  Keep track of the pointers we've allocated. We hold a linked list
  from volk_first_ptr to volk_last_ptr and the number of allocated
  pointers. When allocating a new pointer, we create the pointer with
  malloc, find how it is misaligned, and create a new pointer at the
  alignment boundary. Both of these are stored in the linked list data
  structure. When free, we are passed the aligned pointer and use that
  to look up the original pointer, which we use to actually free the
  entire allocated memory.
*/
unsigned int volk_nptrs = 0;
mbuf* volk_first_ptr = NULL;
mbuf* volk_last_ptr = NULL;

void*
volk_malloc(size_t size, size_t alignment)
{
  // Allocate memory plus enough extra to adjust alignment
  void *ptr = malloc(size + (alignment - 1));
  if(ptr == NULL) {
    free(ptr);
    fprintf(stderr, "VOLK: Error allocating memory (malloc)\n");
    return NULL;
  }

  // Find and return the first aligned boundary of the pointer
  void *aptr = ptr;
  if((unsigned long)ptr % alignment != 0)
    aptr = (void*)((unsigned long)ptr + (alignment - ((unsigned long)ptr % alignment)));

  // Store original pointer and aligned pointers
  mbuf *n = (mbuf*)malloc(sizeof(mbuf));
  n->orig = ptr;
  n->align = aptr;
  n->next = NULL;
  if(volk_first_ptr == NULL) {
    volk_first_ptr = n;
  }
  else {
    volk_last_ptr->next = n;
  }
  volk_last_ptr = n;
  volk_nptrs++;

  return aptr;
}

void volk_free(void *ptr)
{
  unsigned long aptr = (unsigned long)ptr;
  mbuf *prev = volk_first_ptr;
  mbuf *p = volk_first_ptr;

  // Look for the aligned pointer until we either find it or have
  // walked the entire list of allocated pointers
  while(p != NULL) {
    if((unsigned long)(p->align) == aptr) {
      // If the memory is found at the first pointer, move this
      // pointer to the next in the list
      if(p == volk_first_ptr) {
        if(volk_first_ptr == volk_last_ptr)
          volk_last_ptr = NULL;
        volk_first_ptr = p->next;
      }
      // Otherwise, link the previous to the following to skip the
      // struct we're deleting.
      else {
        if(p == volk_last_ptr)
          volk_last_ptr = prev;
        prev->next = p->next;
      }

      // Free the original pointer to remove all memory allocated
      free((void*)p->orig);
      volk_nptrs--;

      // Free the struct to clean up all memory and exit
      free(p);

      return;
    }
    // Not found, update our pointers to look at the next in the list
    prev = p;
    p = p->next;
  }
  fprintf(stderr, "VOLK: tried to free a non-VOLK pointer\n");
}

#endif // _POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600

//#endif // _ISOC11_SOURCE
