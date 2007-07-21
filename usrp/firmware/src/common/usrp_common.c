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

/* 
 * common code for USRP
 */

#include "usrp_common.h"

void init_board (void);

void
init_usrp (void)
{
  CPUCS = bmCLKSPD1;	// CPU runs @ 48 MHz
  CKCON = 0;		// MOVX takes 2 cycles

  // IFCLK is generated internally and runs at 48 MHz; GPIF "master mode"

  IFCONFIG = bmIFCLKSRC | bm3048MHZ | bmIFCLKOE | bmIFCLKPOL | bmIFGPIF;
  SYNCDELAY;

  // configure IO ports (B and D are used by GPIF)

  IOA = bmPORT_A_INITIAL;	// Port A initial state
  OEA = bmPORT_A_OUTPUTS;	// Port A direction register

  IOC = bmPORT_C_INITIAL;	// Port C initial state
  OEC = bmPORT_C_OUTPUTS;	// Port C direction register

  IOE = bmPORT_E_INITIAL;	// Port E initial state
  OEE = bmPORT_E_OUTPUTS;	// Port E direction register


  // REVCTL = bmDYN_OUT | bmENH_PKT;			// highly recommended by docs
  // SYNCDELAY;
  
  // configure end points

  EP1OUTCFG = bmVALID | bmBULK;				SYNCDELAY;
  EP1INCFG  = bmVALID | bmBULK | bmIN;			SYNCDELAY;

  EP2CFG    = bmVALID | bmBULK | bmQUADBUF;		SYNCDELAY;	// 512 quad bulk OUT
  EP4CFG    = 0;					SYNCDELAY;	// disabled
  EP6CFG    = bmVALID | bmBULK | bmQUADBUF | bmIN;	SYNCDELAY;	// 512 quad bulk IN
  EP8CFG    = 0;					SYNCDELAY;	// disabled

  // reset FIFOs

  FIFORESET = bmNAKALL;					SYNCDELAY;
  FIFORESET = 2;					SYNCDELAY;
  // FIFORESET = 4;					SYNCDELAY;
  FIFORESET = 6;					SYNCDELAY;
  // FIFORESET = 8;					SYNCDELAY;
  FIFORESET = 0;					SYNCDELAY;
  
  // configure end point FIFOs

  // let core see 0 to 1 transistion of autoout bit

  EP2FIFOCFG =             bmWORDWIDE;			SYNCDELAY;
  EP2FIFOCFG = bmAUTOOUT | bmWORDWIDE;			SYNCDELAY;
  EP6FIFOCFG = bmAUTOIN  | bmWORDWIDE;			SYNCDELAY;


  // prime the pump 

#if 0
  EP2BCL  = 0x80;		SYNCDELAY;
  EP2BCL  = 0x80;		SYNCDELAY;
  EP2BCL  = 0x80;		SYNCDELAY;
  EP2BCL  = 0x80;		SYNCDELAY;
#endif

  EP0BCH = 0;			SYNCDELAY;

  // arm EP1OUT so we can receive "out" packets (TRM pg 8-8)

  EP1OUTBC = 0;			SYNCDELAY;

  EP2GPIFFLGSEL = 0x01;		SYNCDELAY; // For EP2OUT, GPIF uses EF flag
  EP6GPIFFLGSEL = 0x02;		SYNCDELAY; // For EP6IN,  GPIF uses FF flag

  // set autoin length for EP6
  // FIXME should be f(enumeration)

  EP6AUTOINLENH = (512) >> 8;	SYNCDELAY;  // this is the length for high speed
  EP6AUTOINLENL = (512) & 0xff; SYNCDELAY;

  init_board ();
}

