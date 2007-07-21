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
 * configuration stuff for debugging
 */

/*
 * Define to 0 for normal use of port A, i.e., FPGA control bus.
 * Define to 1 to write trace to port A for scoping with logic analyzer.
 */
#define	UC_TRACE_USING_PORT_A			0


/*
 * Define to 0 for normal use of low 3 bits of port E, i.e., A/D, D/A SLEEP bits.
 * Define to 1 to enable by default driving the GPIF state to the
 * low three bits of port E.
 */
#define UC_START_WITH_GSTATE_OUTPUT_ENABLED	0


/*
 * Define to 1 for normal use (the board really has an FPGA on it).
 * Define to 0 for debug use on board without FPGA.
 */
#define UC_BOARD_HAS_FPGA			1
