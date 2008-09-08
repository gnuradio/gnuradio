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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "u2_init.h"
#include "memory_map.h"
#include "hal_io.h"
#include "pic.h"
#include "nonstdio.h"

//#define DELTA_T  (MASTER_CLK_RATE/2)	// 0.5s (10ns per tick)
#define DELTA_T  5000  		// 5 us (10ns per tick)


void
timer_handler(unsigned irq)
{
  hal_set_timeout(DELTA_T);	// schedule next timeout
  hal_toggle_leds(0x2);
}

int
main(void)
{
  u2_init();

  putstr("blink_leds2\n");
  pic_register_handler(IRQ_TIMER, timer_handler);
  hal_set_timeout(DELTA_T);	// schedule next timeout

  while(1){
    hal_toggle_leds(0x1);
  }

  return 0;
}
