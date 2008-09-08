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

#include "mdelay.h"
#include "memory_map.h"

// Delay about one millisecond.
//
// Need 33,333 cycles at 33 MHz.
// Each time around the loop is 10 cycles
//

#define LOOPCNT(wb_div) (MASTER_CLK_RATE/(wb_div) / 10000)

inline static void
delay_1ms(int loop_count)
{
  int	i;
  for (i = 0; i < loop_count; i++){
    asm volatile ("or  r0, r0, r0\n\
		   or  r0, r0, r0\n\
		   or  r0, r0, r0\n\
		   or  r0, r0, r0\n\
		   or  r0, r0, r0\n\
		   or  r0, r0, r0\n\
		   or  r0, r0, r0\n");
  }
}

// delay about ms milliseconds
void
mdelay(int ms)
{
  static int loop_count = -1;

  if (hwconfig_simulation_p())
    return;

  if (loop_count < 0){
    // set correct loop_count
    static unsigned short lc[8] = {
      0,
      LOOPCNT(1),
      LOOPCNT(2),
      LOOPCNT(3),
      LOOPCNT(4),
      LOOPCNT(5),
      LOOPCNT(6),
      LOOPCNT(7)
    };

    loop_count = lc[hwconfig_wishbone_divisor() & 0x7];
  }

  int i;
  for (i = 0; i < ms; i++)
    delay_1ms(loop_count);
}
