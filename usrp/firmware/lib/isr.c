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

#include "isr.h"
#include "fx2regs.h"
#include "syncdelay.h"

extern xdata unsigned char _standard_interrupt_vector[];
extern xdata unsigned char _usb_autovector[];
extern xdata unsigned char _fifo_gpif_autovector[];

#define LJMP_OPCODE	0x02

/*
 * Hook standard interrupt vector.
 *
 * vector_number is from the SV_<foo> list.
 * addr is the address of the interrupt service routine.
 */
void 
hook_sv (unsigned char vector_number, unsigned short addr)
{
  bit	t;
  
  // sanity checks

  if (vector_number < SV_MIN || vector_number > SV_MAX)
    return;

  if ((vector_number & 0x0f) != 0x03 && (vector_number & 0x0f) != 0x0b)
    return;

  t = EA;
  EA = 0;
  _standard_interrupt_vector[vector_number] = LJMP_OPCODE;
  _standard_interrupt_vector[vector_number + 1] = addr >> 8;
  _standard_interrupt_vector[vector_number + 2] = addr & 0xff;
  EA = t;
}

/*
 * Hook usb interrupt vector.
 *
 * vector_number is from the UV_<foo> list.
 * addr is the address of the interrupt service routine.
 */
void 
hook_uv (unsigned char vector_number, unsigned short addr)
{
  bit	t;
  
  // sanity checks

  if (vector_number < UV_MIN || vector_number > UV_MAX)
    return;

  if ((vector_number & 0x3) != 0)
    return;

  t = EA;
  EA = 0;
  _usb_autovector[vector_number] = LJMP_OPCODE;
  _usb_autovector[vector_number + 1] = addr >> 8;
  _usb_autovector[vector_number + 2] = addr & 0xff;
  EA = t;
}

/*
 * Hook fifo/gpif interrupt vector.
 *
 * vector_number is from the FGV_<foo> list.
 * addr is the address of the interrupt service routine.
 */
void 
hook_fgv (unsigned char vector_number, unsigned short addr)
{
  bit	t;
  
  // sanity checks

  if (vector_number < FGV_MIN || vector_number > FGV_MAX)
    return;

  if ((vector_number & 0x3) != 0)
    return;

  t = EA;
  EA = 0;
  _fifo_gpif_autovector[vector_number] = LJMP_OPCODE;
  _fifo_gpif_autovector[vector_number + 1] = addr >> 8;
  _fifo_gpif_autovector[vector_number + 2] = addr & 0xff;
  EA = t;
}

/*
 * One time call to enable autovectoring for both USB and FIFO/GPIF.
 *
 * This disables all USB and FIFO/GPIF interrupts and clears
 * any pending interrupts too.  It leaves the master USB and FIFO/GPIF
 * interrupts enabled.
 */
void
setup_autovectors (void)
{
  // disable master usb and fifo/gpif interrupt enables
  EIUSB = 0;
  EIEX4 = 0;

  hook_sv (SV_INT_2, (unsigned short) _usb_autovector);
  hook_sv (SV_INT_4, (unsigned short) _fifo_gpif_autovector);

  // disable all fifo interrupt enables
  SYNCDELAY;
  EP2FIFOIE = 0;	SYNCDELAY;
  EP4FIFOIE = 0;	SYNCDELAY;
  EP6FIFOIE = 0;	SYNCDELAY;
  EP8FIFOIE = 0;	SYNCDELAY;

  // clear all pending fifo irqs	
  EP2FIFOIRQ = 0xff;	SYNCDELAY;
  EP4FIFOIRQ = 0xff;	SYNCDELAY;
  EP6FIFOIRQ = 0xff;	SYNCDELAY;
  EP8FIFOIRQ = 0xff;	SYNCDELAY;

  IBNIE  = 0;
  IBNIRQ = 0xff;
  NAKIE  = 0;
  NAKIRQ = 0xff;
  USBIE  = 0;
  USBIRQ = 0xff;
  EPIE   = 0;
  EPIRQ  = 0xff;
  SYNCDELAY;	GPIFIE = 0;		
  SYNCDELAY;	GPIFIRQ = 0xff;
  USBERRIE = 0;
  USBERRIRQ = 0xff;
  CLRERRCNT = 0;
  
  INTSETUP = bmAV2EN | bmAV4EN | bmINT4IN;

  // clear master irq's for usb and fifo/gpif
  EXIF &= ~bmEXIF_USBINT;
  EXIF &= ~bmEXIF_IE4;
  
  // enable master usb and fifo/gpif interrrupts
  EIUSB = 1;
  EIEX4 = 1;
}
