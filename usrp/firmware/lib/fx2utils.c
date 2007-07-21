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

#include "fx2utils.h"
#include "fx2regs.h"
#include "delay.h"

void
fx2_stall_ep0 (void)
{
  EP0CS |= bmEPSTALL;
}

void
fx2_reset_data_toggle (unsigned char ep)
{
  TOGCTL = ((ep & 0x80) >> 3 | (ep & 0x0f));
  TOGCTL |= bmRESETTOGGLE;
}

void
fx2_renumerate (void)
{
  USBCS |= bmDISCON | bmRENUM;

  // mdelay (1500);		// FIXME why 1.5 seconds?
  mdelay (250);			// FIXME why 1.5 seconds?
  
  USBIRQ = 0xff;		// clear any pending USB irqs...
  EPIRQ =  0xff;		//   they're from before the renumeration

  EXIF &= ~bmEXIF_USBINT;

  USBCS &= ~bmDISCON;		// reconnect USB
}
