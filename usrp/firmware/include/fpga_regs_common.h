/* -*- c++ -*- */
/*
 * Copyright 2003,2004 Free Software Foundation, Inc.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.	If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */
#ifndef INCLUDED_FPGA_REGS_COMMON_H
#define INCLUDED_FPGA_REGS_COMMON_H

// This file defines registers common to all FPGA configurations.
// Registers 0 to 31 are reserved for use in this file.


// The FPGA needs to know the rate that samples are coming from and
// going to the A/D's and D/A's.  div = 128e6 / sample_rate

#define	FR_TX_SAMPLE_RATE_DIV	 0
#define	FR_RX_SAMPLE_RATE_DIV	 1

// 2 and 3 are defined in the ATR section

#define	FR_MASTER_CTRL		 4	// master enable and reset controls
#  define  bmFR_MC_ENABLE_TX		(1 << 0)
#  define  bmFR_MC_ENABLE_RX		(1 << 1)
#  define  bmFR_MC_RESET_TX		(1 << 2)
#  define  bmFR_MC_RESET_RX		(1 << 3)

// i/o direction registers for pins that go to daughterboards.
// Setting the bit makes it an output from the FPGA to the d'board.
// top 16 is mask, low 16 is value

#define	FR_OE_0			 5	// slot 0
#define	FR_OE_1			 6
#define	FR_OE_2			 7
#define	FR_OE_3			 8

// i/o registers for pins that go to daughterboards.
// top 16 is a mask, low 16 is value

#define	FR_IO_0			 9	// slot 0
#define	FR_IO_1			10
#define	FR_IO_2			11
#define	FR_IO_3			12

#define	FR_MODE			13
#  define  bmFR_MODE_NORMAL		      0
#  define  bmFR_MODE_LOOPBACK		(1 << 0)	// enable digital loopback
#  define  bmFR_MODE_RX_COUNTING	(1 << 1)	// Rx is counting
#  define  bmFR_MODE_RX_COUNTING_32BIT	(1 << 2)	// Rx is counting with a 32 bit counter
                                                    // low and high 16 bits are multiplexed across channel I and Q


// If the corresponding bit is set, internal FPGA debug circuitry
// controls the i/o pins for the associated bank of daughterboard
// i/o pins.  Typically used for debugging FPGA designs.

#define FR_DEBUG_EN             14
#  define bmFR_DEBUG_EN_TX_A           (1 << 0)        // debug controls TX_A i/o
#  define bmFR_DEBUG_EN_RX_A           (1 << 1)        // debug controls RX_A i/o
#  define bmFR_DEBUG_EN_TX_B           (1 << 2)        // debug controls TX_B i/o
#  define bmFR_DEBUG_EN_RX_B           (1 << 3)        // debug controls RX_B i/o


// If the corresponding bit is set, enable the automatic DC
// offset correction control loop.
//
// The 4 low bits are significant:
//
//   ADC0 = (1 << 0)
//   ADC1 = (1 << 1)
//   ADC2 = (1 << 2)
//   ADC3 = (1 << 3)
//
// This control loop works if the attached daugherboard blocks DC.
// Currently all daughterboards do block DC.  This includes:
// basic rx, dbs_rx, tv_rx, flex_xxx_rx.

#define FR_DC_OFFSET_CL_EN	15			// DC Offset Control Loop Enable


// offset corrections for ADC's and DAC's (2's complement)

#define	FR_ADC_OFFSET_0	 	16
#define	FR_ADC_OFFSET_1		17
#define	FR_ADC_OFFSET_2	 	18
#define	FR_ADC_OFFSET_3		19


// ------------------------------------------------------------------------
// Automatic Transmit/Receive switching
//
// If automatic transmit/receive (ATR) switching is enabled in the
// FR_ATR_CTL register, the presence or absence of data in the FPGA
// transmit fifo selects between two sets of values for each of the 4
// banks of daughterboard i/o pins.
//
// Each daughterboard slot has 3 16-bit registers associated with it:
//   FR_ATR_MASK_*, FR_ATR_TXVAL_* and FR_ATR_RXVAL_*
//
// FR_ATR_MASK_{0,1,2,3}: 
//
//   These registers determine which of the daugherboard i/o pins are
//   affected by ATR switching.  If a bit in the mask is set, the
//   corresponding i/o bit is controlled by ATR, else it's output
//   value comes from the normal i/o pin output register:
//   FR_IO_{0,1,2,3}.
//
// FR_ATR_TXVAL_{0,1,2,3}:
// FR_ATR_RXVAL_{0,1,2,3}:
//
//   If the Tx fifo contains data, then the bits from TXVAL that are
//   selected by MASK are output.  Otherwise, the bits from RXVAL that
//   are selected by MASK are output.
                      
#define FR_ATR_MASK_0		20	// slot 0
#define	FR_ATR_TXVAL_0		21
#define FR_ATR_RXVAL_0		22

#define FR_ATR_MASK_1		23	// slot 1
#define	FR_ATR_TXVAL_1		24
#define FR_ATR_RXVAL_1		25

#define FR_ATR_MASK_2		26	// slot 2
#define	FR_ATR_TXVAL_2		27
#define FR_ATR_RXVAL_2		28

#define FR_ATR_MASK_3		29	// slot 3
#define	FR_ATR_TXVAL_3		30
#define FR_ATR_RXVAL_3		31

// Clock ticks to delay rising and falling edge of T/R signal
#define FR_ATR_TX_DELAY          2
#define FR_ATR_RX_DELAY          3

#endif /* INCLUDED_FPGA_REGS_COMMON_H */
