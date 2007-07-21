/* -*-C-*-
*******************************************************************************
*
* File:         microtune_eval_board_defs.h
* Description:  defines for parallel port control of eval board
*
*******************************************************************************
*/

/*
 * Copyright 2001 Free Software Foundation, Inc.
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

#ifndef _MICROTUNE_EVAL_BOARD_DEFS_H_
#define _MICROTUNE_EVAL_BOARD_DEFS_H_

/*
 * The Microtune 4937DI5 cable modem tuner eval board is controlled
 * by bit banging the PC parallel port.  This file defines the relevant
 * bits.
 *
 * The parallel port has an 8 bit data port (output),
 * an 8 bit control port (output) and
 * an 8 bit status port (input).
 *
 * Not all bits of the control and status ports may be arbitrarily used.
 */


// parallel port data port constants (output)

static const int UT_DP_TX_SDA		= 0x01;	// upstream control bus
static const int UT_DP_TX_SCL		= 0x02;	// upstream control bus
static const int UT_DP_TX_AS		= 0x04;	// upstream control bus
static const int UT_DP_TX_ENABLE 	= 0x08;	// upstream h/w enable
// bits 4,5,6 not used
static const int UT_DP_TUNER_SDA_OUT	= 0x80;	// tuner i2c bus data

// parallel port control port constants (output)

static const int UT_CP_TUNER_SCL	= 0x08;	// tuner i2c bus clock
static const int UT_CP_MUST_BE_ZERO	= 0xf0;	// must be zero

// parallel port status port constants (input)

// bits 0,1,2 not used
static const int UT_SP_TUNER_SCL_LOOP_BACK= 0x08; // inverted SCL loop back
static const int UT_SP_SHOULD_BE_ZERO	= 0x10;	// reads as zero
static const int UT_SP_SHOULD_BE_ONE	= 0x20;	// reads as one
// bit 6 not used
static const int UT_SP_TUNER_SDA_IN	= 0x80;


#endif /* _MICROTUNE_EVAL_BOARD_DEFS_H_ */
