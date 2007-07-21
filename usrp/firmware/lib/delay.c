/* -*- c++ -*- */
/*
 * Copyright 2003 Free Software Foundation, Inc.
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

/*
 * Delay approximately 1 microsecond (including overhead in udelay).
 */
static void
udelay1 (void) _naked
{
  _asm				; lcall that got us here took 4 bus cycles
	ret			; 4 bus cycles
  _endasm;
}

/*
 * delay for approximately usecs microseconds
 */
void
udelay (unsigned char usecs)
{
  do {
    udelay1 ();
  } while (--usecs != 0);
}


/*
 * Delay approximately 1 millisecond.
 * We're running at 48 MHz, so we need 48,000 clock cycles.
 *
 * Note however, that each bus cycle takes 4 clock cycles (not obvious,
 * but explains the factor of 4 problem below).
 */
static void
mdelay1 (void) _naked
{
  _asm
	mov	dptr,#(-1200 & 0xffff)
002$:	
	inc	dptr		; 3 bus cycles
	mov	a, dpl		; 2 bus cycles
	orl	a, dph		; 2 bus cycles
	jnz	002$		; 3 bus cycles

	ret
  _endasm;
}

void
mdelay (unsigned int msecs)
{
  do {
    mdelay1 ();
  } while (--msecs != 0);
}

	
