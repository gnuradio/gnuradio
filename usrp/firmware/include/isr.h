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

#ifndef _ISR_H_
#define _ISR_H_

/*
 * ----------------------------------------------------------------
 *	routines for managing interrupt services routines
 * ----------------------------------------------------------------
 */

/*
 * The FX2 has three discrete sets of interrupt vectors.
 * The first set is the standard 8051 vector (13 8-byte entries).
 * The second set is USB interrupt autovector (32 4-byte entries).
 * The third set is the FIFO/GPIF autovector (14 4-byte entries).
 *
 * Since all the code we're running in the FX2 is ram based, we
 * forego the typical "initialize the interrupt vectors at link time"
 * strategy, in favor of calls at run time that install the correct
 * pointers to functions.
 */

/*
 * Standard Vector numbers
 */

#define	SV_INT_0		0x03
#define	SV_TIMER_0		0x0b
#define	SV_INT_1		0x13
#define	SV_TIMER_1		0x1b
#define	SV_SERIAL_0		0x23
#define	SV_TIMER_2		0x2b
#define	SV_RESUME		0x33
#define	SV_SERIAL_1		0x3b
#define	SV_INT_2		0x43		// (INT_2) points at USB autovector
#define	SV_I2C			0x4b
#define	SV_INT_4		0x53		// (INT_4) points at FIFO/GPIF autovector
#define	SV_INT_5		0x5b
#define	SV_INT_6		0x63

#define	SV_MIN			SV_INT_0
#define	SV_MAX			SV_INT_6

/*
 * USB Auto Vector numbers
 */

#define	UV_SUDAV		0x00
#define	UV_SOF			0x04
#define	UV_SUTOK		0x08
#define	UV_SUSPEND		0x0c
#define	UV_USBRESET		0x10
#define	UV_HIGHSPEED		0x14
#define	UV_EP0ACK		0x18
#define	UV_SPARE_1C		0x1c
#define	UV_EP0IN		0x20
#define	UV_EP0OUT		0x24
#define	UV_EP1IN		0x28
#define	UV_EP1OUT		0x2c
#define	UV_EP2			0x30
#define	UV_EP4			0x34
#define	UV_EP6			0x38
#define	UV_EP8			0x3c
#define	UV_IBN			0x40
#define	UV_SPARE_44		0x44
#define	UV_EP0PINGNAK		0x48
#define	UV_EP1PINGNAK		0x4c
#define	UV_EP2PINGNAK		0x50
#define	UV_EP4PINGNAK		0x54
#define	UV_EP6PINGNAK		0x58
#define	UV_EP8PINGNAK		0x5c
#define	UV_ERRLIMIT		0x60
#define	UV_SPARE_64		0x64
#define	UV_SPARE_68		0x68
#define	UV_SPARE_6C		0x6c
#define	UV_EP2ISOERR		0x70
#define	UV_EP4ISOERR		0x74
#define	UV_EP6ISOERR		0x78
#define	UV_EP8ISOERR		0x7c

#define	UV_MIN			UV_SUDAV
#define	UV_MAX			UV_EP8ISOERR

/*
 * FIFO/GPIF Auto Vector numbers
 */

#define	FGV_EP2PF		0x00
#define	FGV_EP4PF		0x04
#define	FGV_EP6PF		0x08
#define	FGV_EP8PF		0x0c
#define	FGV_EP2EF		0x10
#define	FGV_EP4EF		0x14
#define	FGV_EP6EF		0x18
#define	FGV_EP8EF		0x1c
#define	FGV_EP2FF		0x20
#define	FGV_EP4FF		0x24
#define	FGV_EP6FF		0x28
#define	FGV_EP8FF		0x2c
#define	FGV_GPIFDONE		0x30
#define	FGV_GPIFWF		0x34

#define	FGV_MIN			FGV_EP2PF
#define	FGV_MAX			FGV_GPIFWF


/*
 * Hook standard interrupt vector.
 *
 * vector_number is from the SV_<foo> list above.
 * addr is the address of the interrupt service routine.
 */
void hook_sv (unsigned char vector_number, unsigned short addr);

/*
 * Hook usb interrupt vector.
 *
 * vector_number is from the UV_<foo> list above.
 * addr is the address of the interrupt service routine.
 */
void hook_uv (unsigned char vector_number, unsigned short addr);

/*
 * Hook fifo/gpif interrupt vector.
 *
 * vector_number is from the FGV_<foo> list above.
 * addr is the address of the interrupt service routine.
 */
void hook_fgv (unsigned char vector_number, unsigned short addr);

/*
 * One time call to enable autovectoring for both USB and FIFO/GPIF
 */
void setup_autovectors (void);


/*
 * Must be called in each usb interrupt handler
 */
#define	clear_usb_irq()			\
	EXIF &= ~bmEXIF_USBINT;		\
	INT2CLR = 0

/*
 * Must be calledin each fifo/gpif interrupt handler
 */
#define	clear_fifo_gpif_irq()		\
	EXIF &= ~bmEXIF_IE4;		\
	INT4CLR = 0

#endif /* _ISR_H_ */
