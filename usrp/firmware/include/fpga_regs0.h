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

#ifndef _FPGA_REGS0_H_
#define _FPGA_REGS0_H_

#define FR_RX_FREQ_0	   0
#define FR_RX_FREQ_1	   1
#define FR_RX_FREQ_2	   2
#define FR_RX_FREQ_3	   3
#define FR_TX_FREQ_0	   4
#define FR_TX_FREQ_1	   5
#define FR_TX_FREQ_2	   6
#define FR_TX_FREQ_3	   7
#define FR_COMBO	   8


#define FR_ADC_CLK_DIV   128	// pseudo regs mapped to FR_COMBO by f/w
#define FR_EXT_CLK_DIV	 129	
#define FR_INTERP	 130
#define FR_DECIM	 131

#endif
