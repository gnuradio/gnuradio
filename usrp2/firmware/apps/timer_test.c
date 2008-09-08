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

#include "u2_init.h"
#include "memory_map.h"
#include "hal_io.h"
#include "buffer_pool.h"
#include "pic.h"
#include "nonstdio.h"


#define DELTA_T  500  		// 5 us (10ns per tick)


void 
timer_handler(unsigned irq)
{
  int t = timer_regs->time;
  timer_regs->time = t + DELTA_T;

  putstr("Tick: ");
  puthex_nl(t);
}

int
main(void)
{
  u2_init();

  // setup timer

  putstr("Setting up timer\n");
  pic_register_handler(IRQ_TIMER, timer_handler);

  int t = timer_regs->time;
  timer_regs->time = t + DELTA_T;

  while (1)
    ;

  putstr("Done Testing\n");
  
  hal_finish();
  return 1;
}
