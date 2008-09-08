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
#include "bool.h"
#include "nonstdio.h"
#include "hal_io.h"
#include "mdelay.h"


static void
write_bufs(void)
{
  int	i, n;
  int	counter = 0;

  for (n = 0; n < NBUFFERS; n++){
    volatile int *p = buffer_ram(n);
    for (i = 0; i < BP_NLINES; i++)
      p[i] = counter++;
  }
}

// return number of errors detected
static int
check_bufs(void)
{
  int	i, n;
  int	counter = 0;
  int	nerrors = 0;

  for (n = 0; n < NBUFFERS; n++){
    volatile int *p = buffer_ram(n);
    for (i = 0; i < BP_NLINES; i++, counter++){
      int rd = p[i];
      if (rd != counter){
	putchar('b');
	putchar(n + '0');
	putchar('[');
	puthex16(i);
	putstr("] exp: ");
	puthex32(counter);
	putstr(" got: ");
	puthex32_nl(rd);
	nerrors++;
      }
    }
  }
  return nerrors;
}


int
main(void)
{
  u2_init();

  output_regs->leds = 0;

  write_bufs();
  int nerrors = check_bufs();

  if (nerrors == 0){
    output_regs->leds = 0x3;		// leds on  -> PASS
    putstr("PASS\n");
  }
  else {
    output_regs->leds = 0x0;		// leds off -> FAIL
    putstr("FAIL\n");
  }

  hal_finish();
  return 0;
}
