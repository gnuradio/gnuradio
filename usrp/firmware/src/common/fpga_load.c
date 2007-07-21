/* 
 * USRP - Universal Software Radio Peripheral
 *
 * Copyright (C) 2003 Free Software Foundation, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Boston, MA  02110-1301  USA
 */

#include "usrp_common.h"
#include "fpga_load.h"
#include "delay.h"

/*
 * setup altera FPGA serial load (PS).
 *
 * On entry:
 *	don't care
 *
 * On exit:
 *	ALTERA_DCLK    = 0
 *	ALTERA_NCONFIG = 1
 *	ALTERA_NSTATUS = 1 (input)
 */
unsigned char
fpga_load_begin (void)
{
  USRP_ALTERA_CONFIG &= ~bmALTERA_BITS;		// clear all bits (NCONFIG low)
  udelay (40);					// wait 40 us
  USRP_ALTERA_CONFIG |= bmALTERA_NCONFIG;	// set NCONFIG high

  if (UC_BOARD_HAS_FPGA){
    // FIXME should really cap this loop with a counter so we
    //   don't hang forever on a hardware failure.
    while ((USRP_ALTERA_CONFIG & bmALTERA_NSTATUS) == 0) // wait for NSTATUS to go high
      ;
  }

  // ready to xfer now

  return 1;
}

/*
 * clock out the low bit of bits.
 *
 * On entry:
 *	ALTERA_DCLK    = 0
 *	ALTERA_NCONFIG = 1
 *	ALTERA_NSTATUS = 1 (input)
 *
 * On exit:
 *	ALTERA_DCLK    = 0
 *	ALTERA_NCONFIG = 1
 *	ALTERA_NSTATUS = 1 (input)
 */


#if 0

static void
clock_out_config_byte (unsigned char bits)
{
  unsigned char i;

  // clock out configuration byte, least significant bit first

  for (i = 0; i < 8; i++){

    USRP_ALTERA_CONFIG = ((USRP_ALTERA_CONFIG & ~bmALTERA_DATA0) | ((bits & 1) ? bmALTERA_DATA0 : 0));
    USRP_ALTERA_CONFIG |= bmALTERA_DCLK;		/* set DCLK to 1 */
    USRP_ALTERA_CONFIG &= ~bmALTERA_DCLK;		/* set DCLK to 0 */

    bits = bits >> 1;
  }
}
	
#else

static void 
clock_out_config_byte (unsigned char bits) _naked
{
    _asm
	mov	a, dpl
	
	rrc	a
	mov	_bitALTERA_DATA0,c
	setb	_bitALTERA_DCLK
	clr	_bitALTERA_DCLK
	
	rrc	a
	mov	_bitALTERA_DATA0,c
	setb	_bitALTERA_DCLK
	clr	_bitALTERA_DCLK
	
	rrc	a
	mov	_bitALTERA_DATA0,c
	setb	_bitALTERA_DCLK
	clr	_bitALTERA_DCLK
	
	rrc	a
	mov	_bitALTERA_DATA0,c
	setb	_bitALTERA_DCLK
	clr	_bitALTERA_DCLK
	
	rrc	a
	mov	_bitALTERA_DATA0,c
	setb	_bitALTERA_DCLK
	clr	_bitALTERA_DCLK
	
	rrc	a
	mov	_bitALTERA_DATA0,c
	setb	_bitALTERA_DCLK
	clr	_bitALTERA_DCLK
	
	rrc	a
	mov	_bitALTERA_DATA0,c
	setb	_bitALTERA_DCLK
	clr	_bitALTERA_DCLK
	
	rrc	a
	mov	_bitALTERA_DATA0,c
	setb	_bitALTERA_DCLK
	clr	_bitALTERA_DCLK
	
	ret	

    _endasm;
}

#endif

static void
clock_out_bytes (unsigned char bytecount,
		 unsigned char xdata *p)
{
  while (bytecount-- > 0)
    clock_out_config_byte (*p++);
}

/*
 * Transfer block of bytes from packet to FPGA serial configuration port
 *
 * On entry:
 *	ALTERA_DCLK    = 0
 *	ALTERA_NCONFIG = 1
 *	ALTERA_NSTATUS = 1 (input)
 *
 * On exit:
 *	ALTERA_DCLK    = 0
 *	ALTERA_NCONFIG = 1
 *	ALTERA_NSTATUS = 1 (input)
 */
unsigned char
fpga_load_xfer (xdata unsigned char *p, unsigned char bytecount)
{
  clock_out_bytes (bytecount, p);
  return 1;
}

/*
 * check for successful load...
 */
unsigned char
fpga_load_end (void)
{
  unsigned char status = USRP_ALTERA_CONFIG;

  if (!UC_BOARD_HAS_FPGA)			// always true if we don't have FPGA
    return 1;

  if ((status & bmALTERA_NSTATUS) == 0)		// failed to program
    return 0;

  if ((status & bmALTERA_CONF_DONE) == bmALTERA_CONF_DONE)
    return 1;					// everything's cool

  // I don't think this should happen.  It indicates that
  // programming is still in progress.

  return 0;
}
