/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef INCLUDED_AD9777_REGS_H
#define INCLUDED_AD9777_REGS_H

#define R0_SW_RESET	(1 << 5)
#define	R0_SLEEP	(1 << 4)
#define	R0_POWER_DN	(1 << 3)
#define	R0_1R		(1 << 2)
#define R0_2R		(0 << 2)
#define	R0_PLL_LOCKED	(1 << 1)

#define	R1_INTERP_1X	0x00
#define	R1_INTERP_2X	0x40
#define	R1_INTERP_4X	0x80
#define R1_INTERP_8X	0xC0
#define	R1_MOD_NONE	0x00
#define	R1_MOD_FS_2	0x10		// Fs/2
#define	R1_MOD_FS_4	0x20		// Fs/4
#define	R1_MOD_FS_8	0x30		// Fs/8
#define R1_ZERO_STUFF	(1 << 3)	// N.B., doubles output rate
#define	R1_REAL_MIX	(1 << 2)
#define	R1_CMPLX_MIX	(0 << 2)
#define	R1_POS_EXP	(1 << 1)	// exp(+jwt)
#define	R1_NEG_EXP	(0 << 1)	// exp(-jwt)
#define	R1_DATACLK_OUT	(1 << 0)

#define	R2_2S_COMP	(0 << 7)
#define R2_2PORT_MODE	(0 << 6)
#define	R2_1PORT_MODE	(1 << 6)

#define	R3_PLL_DIV_1	0x00
#define	R3_PLL_DIV_2	0x01
#define	R3_PLL_DIV_4	0x02
#define	R3_PLL_DIV_8	0x03

#define	R4_PLL_ON	(1 << 7)
#define	R4_CP_MANUAL	(1 << 6)
#define	R4_CP_AUTO	(0 << 6)
#define	R4_CP_50uA	(0x00 | R4_CP_MANUAL)
#define	R4_CP_100uA	(0x01 | R4_CP_MANUAL)
#define	R4_CP_200uA	(0x02 | R4_CP_MANUAL)
#define	R4_CP_400uA	(0x03 | R4_CP_MANUAL)
#define	R4_CP_800uA	(0x07 | R4_CP_MANUAL)

#define R5_I_FINE_GAIN(g)    (g)		// 8-bits
#define R6_I_COARSE_GAIN(g)  ((g) & 0xf)	// low 4-bits

#define	R9_Q_FINE_GAIN(g)    (g)		// 8-bits
#define	R10_Q_COARSE_GAIN(g) ((g) & 0xf)	// low 4-bits


// FIXME more registers for offset and gain control...


#endif /* INCLUDED_AD9777_REGS_H */
