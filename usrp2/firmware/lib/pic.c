/* -*- c -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "pic.h"
#include "hal_io.h"
#include "memory_map.h"


#define NVECTORS 8

/*
 * Our secondary interrupt vector.
 */
irq_handler_t pic_vector[NVECTORS] = {
  nop_handler,
  nop_handler,
  nop_handler,
  nop_handler,
  nop_handler,
  nop_handler,
  nop_handler,
  nop_handler
};


void
pic_init(void)
{
  // uP is level triggered

  pic_regs->mask = ~0;				       // mask all interrupts
  pic_regs->edge_enable = PIC_TIMER_INT | PIC_PHY_INT;
  pic_regs->polarity = ~0 & ~PIC_PHY_INT;	       // rising edge
  pic_regs->pending = ~0;			       // clear all pending ints
}

/*
 * This magic gets pic_interrupt_handler wired into the
 * system interrupt handler with the appropriate prologue and
 * epilogue.
 */
void pic_interrupt_handler() __attribute__ ((interrupt_handler));

void pic_interrupt_handler()
{
  // pending and not masked interrupts
  int live = pic_regs->pending & ~pic_regs->mask;

  // FIXME loop while there are interrupts to service.
  //   That will reduce our overhead.

  // handle the first one set
  int i;
  int mask;
  for (i=0, mask=1; i < NVECTORS; i++, mask <<= 1){
    if (mask & live){		// handle this one
      // puthex_nl(i);
      (*pic_vector[i])(i);
      pic_regs->pending = mask;	// clear pending interrupt
      return;
    }
  }
}

void
pic_register_handler(unsigned irq, irq_handler_t handler)
{
  if (irq >= NVECTORS)
    return;
  pic_vector[irq] = handler;

  pic_regs->mask &= ~IRQ_TO_MASK(irq);
}

void
nop_handler(unsigned irq)
{
  // nop
}
