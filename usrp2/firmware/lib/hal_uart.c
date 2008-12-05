/* -*- c -*- */
/*
 * Copyright 2007,2008 Free Software Foundation, Inc.
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

#include "hal_uart.h"
#include "hal_io.h"
#include "memory_map.h"

// First pass, no interrupts

// Replaced with divisors.py which generates best divisor
//#define CALC_DIVISOR(rate) (WISHBONE_CLK_RATE / ((rate) * 16))

#define NSPEEDS 6
#define	MAX_WB_DIV 4

static const uint16_t
divisor_table[MAX_WB_DIV+1][NSPEEDS] = {
  {   2,   2,   2,   2,  2,  2},   // 0: can't happen
  { 651, 326, 163, 109, 54, 27 },  // 1: 100 MHz
  { 326, 163,  81,  54, 27, 14 },  // 2:  50 MHz
  { 217, 109,  54,  36, 18,  9 },  // 3:  33.3333 MHz
  { 163,  81,  41,  27, 14,  7 },  // 4:  25 MHz
};

#define u uart_regs

void
hal_uart_init(void)
{
  u->clkdiv = 217;  // 230400 bps
}

void
hal_uart_putc(int ch)
{
  if (ch == '\n')		// FIXME for now map \n -> \r\n
    hal_uart_putc('\r');

  while (u->txlevel == 0)	 // wait for fifo to have space
    ;

  u->txchar = ch;
}

void
hal_uart_putc_nowait(int ch)
{
  if (ch == '\n')		// FIXME for now map \n -> \r\n
    hal_uart_putc('\r');

  if(u->txlevel)   // If fifo has space
    u->txchar = ch;
}

int
hal_uart_getc(void)
{
  while ((u->rxlevel) == 0)  // wait for data to be ready
    ;

  return u->rxchar;
}
