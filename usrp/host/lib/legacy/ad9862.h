/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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

#ifndef INCLUDED_AD9862_H
#define INCLUDED_AD9862_H

/*
 * Analog Devices AD9862 registers and some fields
 */

#define BEGIN_AD9862	namespace ad9862 {
#define	END_AD962	}
#define	DEF static const int

BEGIN_AD9862;

DEF REG_GENERAL		=  0;
DEF REG_RX_PWR_DN	=  1;
DEF	RX_PWR_DN_VREF_DIFF		= (1 << 7);
DEF	RX_PWR_DN_VREF			= (1 << 6);
DEF	RX_PWR_DN_RX_DIGIGAL		= (1 << 5);
DEF	RX_PWR_DN_RX_B			= (1 << 4);
DEF	RX_PWR_DN_RX_A			= (1 << 3);
DEF	RX_PWR_DN_BUF_B			= (1 << 2);
DEF	RX_PWR_DN_BUF_A			= (1 << 1);
DEF	RX_PWR_DN_ALL			= (1 << 0);

DEF REG_RX_A		=  2;	// bypass input buffer / RxPGA
DEF REG_RX_B		=  3;	// pypass input buffer / RxPGA
DEF	RX_X_BYPASS_INPUT_BUFFER	= (1 << 7);

DEF REG_RX_MISC		=  4;
DEF	RX_MISC_HS_DUTY_CYCLE		= (1 << 2);
DEF	RX_MISC_SHARED_REF		= (1 << 1);
DEF	RX_MISC_CLK_DUTY		= (1 << 0);

DEF REG_RX_IF		=  5;
DEF	RX_IF_THREE_STATE		= (1 << 4);
DEF	RX_IF_USE_CLKOUT1		= (0 << 3);	
DEF	RX_IF_USE_CLKOUT2		= (1 << 3);	// aka Rx Retime
DEF	RX_IF_2S_COMP			= (1 << 2);
DEF	RX_IF_INV_RX_SYNC		= (1 << 1);
DEF	RX_IF_MUX_OUT			= (1 << 0);

DEF REG_RX_DIGITAL	=  6;
DEF	RX_DIGITAL_2_CHAN		= (1 << 3);
DEF	RX_DIGITAL_KEEP_MINUS_VE	= (1 << 2);
DEF	RX_DIGITAL_HILBERT		= (1 << 1);
DEF	RX_DIGITAL_DECIMATE		= (1 << 0);

DEF REG_RESERVED_7	=  7;

DEF REG_TX_PWR_DN	=  8;
DEF	TX_PWR_DN_ALT_TIMING_MODE	= (1 << 5);
DEF	TX_PWR_DN_TX_OFF_ENABLE		= (1 << 4);
DEF	TX_PWR_DN_TX_DIGITAL		= (1 << 3);
DEF	TX_PWR_DN_TX_ANALOG_B		= 0x4;
DEF	TX_PWR_DN_TX_ANALOG_A		= 0x2;
DEF	TX_PWR_DN_TX_ANALOG_BOTH	= 0x7;

DEF REG_RESERVED_9	=  9;

DEF REG_TX_A_OFFSET_LO	= 10;
DEF REG_TX_A_OFFSET_HI	= 11;
DEF REG_TX_B_OFFSET_LO	= 12;
DEF REG_TX_B_OFFSET_HI	= 13;

DEF REG_TX_A_GAIN	= 14;	// fine trim for matching
DEF REG_TX_B_GAIN	= 15;	// fine trim for matching
DEF	TX_X_GAIN_COARSE_FULL		= (3 << 6);
DEF	TX_X_GAIN_COARSE_1_HALF		= (1 << 6);
DEF	TX_X_GAIN_COARSE_1_ELEVENTH	= (0 << 6);

DEF REG_TX_PGA		= 16;	// 20 dB continuous gain in 0.1 dB steps
				// 0x00 = min gain (-20 dB)
				// 0xff = max gain (  0 dB)

DEF REG_TX_MISC		= 17;
DEF	TX_MISC_SLAVE_ENABLE		= (1 << 1);
DEF	TX_MISC_TX_PGA_FAST		= (1 << 0);

DEF REG_TX_IF		= 18;
DEF	TX_IF_USE_CLKOUT2		= (0 << 6);
DEF	TX_IF_USE_CLKOUT1		= (1 << 6);	// aka Tx Retime
DEF	TX_IF_I_FIRST			= (0 << 5);
DEF	TX_IF_Q_FIRST			= (1 << 5);
DEF	TX_IF_INV_TX_SYNC		= (1 << 4);
DEF	TX_IF_2S_COMP			= (1 << 3);
DEF	TX_IF_INVERSE_SAMPLE		= (1 << 2);
DEF	TX_IF_TWO_EDGES			= (1 << 1);
DEF	TX_IF_INTERLEAVED		= (1 << 0);

DEF REG_TX_DIGITAL	= 19;
DEF	TX_DIGITAL_2_DATA_PATHS		= (1 << 4);
DEF	TX_DIGITAL_KEEP_NEGATIVE	= (1 << 3);
DEF	TX_DIGITAL_HILBERT		= (1 << 2);
DEF	TX_DIGITAL_INTERPOLATE_NONE	= 0x0;
DEF	TX_DIGITAL_INTERPOLATE_2X	= 0x1;
DEF	TX_DIGITAL_INTERPOLATE_4X	= 0x2;

DEF REG_TX_MODULATOR	= 20;
DEF	TX_MODULATOR_NEG_FINE_TUNE	= (1 << 5);
DEF	TX_MODULATOR_DISABLE_NCO	= (0 << 4);
DEF	TX_MODULATOR_ENABLE_NCO		= (1 << 4);	// aka Fine Mode
DEF	TX_MODULATOR_REAL_MIX_MODE	= (1 << 3);
DEF	TX_MODULATOR_NEG_COARSE_TUNE	= (1 << 2);
DEF	TX_MODULATOR_COARSE_MODULATION_NONE	= 0x0;
DEF	TX_MODULATOR_COARSE_MODULATION_F_OVER_4	= 0x1;
DEF	TX_MODULATOR_COARSE_MODULATION_F_OVER_8 = 0x2;
DEF	TX_MODULATOR_CM_MASK		        = 0x7;


DEF REG_TX_NCO_FTW_7_0	= 21;
DEF REG_TX_NCO_FTW_15_8	= 22;
DEF REG_TX_NCO_FTW_23_16= 23;

DEF REG_DLL		= 24;
DEF	DLL_DISABLE_INTERNAL_XTAL_OSC	= (1 << 6);	// aka Input Clock Ctrl
DEF	DLL_ADC_DIV2			= (1 << 5);
DEF	DLL_MULT_1X			= (0 << 3);
DEF	DLL_MULT_2X			= (1 << 3);
DEF	DLL_MULT_4X			= (2 << 3);
DEF	DLL_PWR_DN			= (1 << 2);
// undefined bit			= (1 << 1);
DEF	DLL_FAST			= (1 << 0);

DEF REG_CLKOUT		= 25;
DEF	CLKOUT2_EQ_DLL			= (0 << 6);
DEF	CLKOUT2_EQ_DLL_OVER_2		= (1 << 6);
DEF	CLKOUT2_EQ_DLL_OVER_4		= (2 << 6);
DEF	CLKOUT2_EQ_DLL_OVER_8		= (3 << 6);
DEF	CLKOUT_INVERT_CLKOUT2		= (1 << 5);
DEF	CLKOUT_DISABLE_CLKOUT2		= (1 << 4);
// undefined bit			= (1 << 3);
// undefined bit			= (1 << 2);
DEF	CLKOUT_INVERT_CLKOUT1		= (1 << 1);
DEF	CLKOUT_DISABLE_CLKOUT1		= (1 << 0);

DEF REG_AUX_ADC_A2_LO	= 26;
DEF REG_AUX_ADC_A2_HI	= 27;
DEF REG_AUX_ADC_A1_LO	= 28;
DEF REG_AUX_ADC_A1_HI	= 29;
DEF REG_AUX_ADC_B2_LO	= 30;
DEF REG_AUX_ADC_B2_HI	= 31;
DEF REG_AUX_ADC_B1_LO	= 32;
DEF REG_AUX_ADC_B1_HI	= 33;

DEF REG_AUX_ADC_CTRL	= 34;
DEF	AUX_ADC_CTRL_AUX_SPI		= (1 << 7);
DEF	AUX_ADC_CTRL_SELBNOTA		= (1 << 6);
DEF	AUX_ADC_CTRL_REFSEL_B		= (1 << 5);
DEF	AUX_ADC_CTRL_SELECT_B2 		= (0 << 4);
DEF	AUX_ADC_CTRL_SELECT_B1		= (1 << 4);
DEF	AUX_ADC_CTRL_START_B		= (1 << 3);
DEF	AUX_ADC_CTRL_REFSEL_A		= (1 << 2);
DEF	AUX_ADC_CTRL_SELECT_A2		= (0 << 1);
DEF	AUX_ADC_CTRL_SELECT_A1		= (1 << 1);
DEF	AUX_ADC_CTRL_START_A   		= (1 << 0);

DEF REG_AUX_ADC_CLK	= 35;
DEF	AUX_ADC_CLK_CLK_OVER_4		= (1 << 0);

DEF REG_AUX_DAC_A	= 36;
DEF REG_AUX_DAC_B	= 37;
DEF REG_AUX_DAC_C	= 38;

DEF REG_AUX_DAC_UPDATE	= 39;
DEF	AUX_DAC_UPDATE_SLAVE_ENABLE	= (1 << 7);
DEF	AUX_DAC_UPDATE_C		= (1 << 2);
DEF	AUX_DAC_UPDATE_B		= (1 << 1);
DEF	AUX_DAC_UPDATE_A		= (1 << 0);

DEF REG_AUX_DAC_PWR_DN	= 40;
DEF	AUX_DAC_PWR_DN_C		= (1 << 2);
DEF	AUX_DAC_PWR_DN_B		= (1 << 1);
DEF	AUX_DAC_PWR_DN_A		= (1 << 0);

DEF REG_AUX_DAC_CTRL	= 41;
DEF	AUX_DAC_CTRL_INV_C		= (1 << 4);
DEF	AUX_DAC_CTRL_INV_B		= (1 << 2);
DEF	AUX_DAC_CTRL_INV_A		= (1 << 0);

DEF REG_SIGDELT_LO	= 42;
DEF REG_SIGDELT_HI	= 43;

// 44 to 48 reserved

DEF REG_ADC_LOW_PWR_LO	= 49;
DEF REG_ADC_LOW_PWR_HI	= 50;

// 51 to 62 reserved

DEF REG_CHIP_ID		= 63;


END_AD962;

#undef DEF
#undef BEGIN_AD9862
#undef END_AD962

#endif /* INCLUDED_AD9862_H */
