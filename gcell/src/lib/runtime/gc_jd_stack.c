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

#include "gc_jd_stack.h"
#include "memory_barrier.h"


void 
gc_jd_stack_init(gc_jd_stack_t *stack)
{
  stack->top = 0;
}
  

#ifdef __powerpc64__  // 64-bit mode

void 
gc_jd_stack_push(gc_jd_stack_t *stack, gc_job_desc_t *item)
{
  gc_eaddr_t	top;
  gc_eaddr_t	item_ea = ptr_to_ea(item);
  unsigned int	done;

  do {
    top = __ldarx(&stack->top);
    item->sys.next = top;
    smp_wmb();	      // order store of item->next before store of stack->top
    done = __stdcx(&stack->top, item_ea);
  } while (unlikely(done == 0));
}

gc_job_desc_t *
gc_jd_stack_pop(gc_jd_stack_t *stack)
{
  gc_eaddr_t	s;
  gc_eaddr_t	t;
  unsigned int	done;

  do {
    s  = __ldarx(&stack->top);
    if (s == 0)			/* stack's empty */
      return 0;
    t = ((gc_job_desc_t *) ea_to_ptr(s))->sys.next;
    done = __stdcx(&stack->top, t);
  } while (unlikely(done == 0));

  return ea_to_ptr(s);
}

#else  // 32-bit mode

/*
 * In 32-bit mode, gc_eaddr's will have the top 32-bits zero.
 * The ldarx/stdcx instructions aren't available in 32-bit mode,
 * thus we use lwarx/stwcx on the low 32-bits of the 64-bit addresses.
 * Since we're big-endian, the low 32-bits are at word offset 1.
 */
void 
gc_jd_stack_push(gc_jd_stack_t *stack, gc_job_desc_t *item)
{
  gc_eaddr_t	top;
  unsigned int	done;

  do {
    top = __lwarx((int32_t *)(&stack->top) + 1);
    item->sys.next = top;
    smp_wmb();	      // order store of item->sys.next before store of stack->top
    done = __stwcx((int32_t *)(&stack->top) + 1, item);
  } while (unlikely(done == 0));
}

gc_job_desc_t *
gc_jd_stack_pop(gc_jd_stack_t *stack)
{
  gc_eaddr_t	s;
  gc_eaddr_t	t;
  unsigned int	done;

  do {
    s  = __lwarx((int32_t *)(&stack->top) + 1);
    if (s == 0)			/* stack's empty */
      return 0;
    t = ((gc_job_desc_t *) ea_to_ptr(s))->sys.next;
    done = __stwcx((int32_t *)(&stack->top) + 1, (uint32_t) t);
  } while (unlikely(done == 0));

  return ea_to_ptr(s);
}

#endif
