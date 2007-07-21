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
 * These are the register definitions for the Rev 1 USRP prototype
 * The Rev 1 is the version with the AD9862's and daughterboards
 */

#ifndef _USRP_REV1_REGS_H_
#define _USRP_REV1_REGS_H_

#include "fx2regs.h"

/*
 * Port A (bit addressable):
 */

#define USRP_PA			IOA		// Port A
#define	USRP_PA_OE		OEA		// Port A direction register

#define bmPA_S_CLK		bmBIT0		// SPI serial clock
#define	bmPA_S_DATA_TO_PERIPH	bmBIT1		// SPI SDI (peripheral rel name)
#define bmPA_S_DATA_FROM_PERIPH	bmBIT2		// SPI SDO (peripheral rel name)
#define bmPA_SEN_FPGA		bmBIT3		// serial enable for FPGA (active low)
#define	bmPA_SEN_CODEC_A	bmBIT4		// serial enable AD9862 A (active low)
#define	bmPA_SEN_CODEC_B	bmBIT5		// serial enable AD9862 B (active low)
//#define bmPA_FX2_2		bmBIT6		// misc pin to FPGA (overflow)
//#define bmPA_FX2_3		bmBIT7		// misc pin to FPGA (underflow)
#define	bmPA_RX_OVERRUN		bmBIT6		// misc pin to FPGA (overflow)
#define	bmPA_TX_UNDERRUN	bmBIT7		// misc pin to FPGA (underflow)


sbit at 0x80+0 bitS_CLK;		// 0x80 is the bit address of PORT A
sbit at 0x80+1 bitS_OUT;		// out from FX2 point of view
sbit at 0x80+2 bitS_IN;			// in from FX2 point of view


/* all outputs except S_DATA_FROM_PERIPH, FX2_2, FX2_3 */

#define	bmPORT_A_OUTPUTS  (bmPA_S_CLK			\
			   | bmPA_S_DATA_TO_PERIPH	\
			   | bmPA_SEN_FPGA		\
			   | bmPA_SEN_CODEC_A		\
			   | bmPA_SEN_CODEC_B		\
			   )

#define	bmPORT_A_INITIAL   (bmPA_SEN_FPGA | bmPA_SEN_CODEC_A | bmPA_SEN_CODEC_B)


/* Port B: GPIF	FD[7:0]			*/

/*
 * Port C (bit addressable):
 *    5:1 FPGA configuration
 */

#define	USRP_PC			IOC		// Port C
#define	USRP_PC_OE		OEC		// Port C direction register

#define	USRP_ALTERA_CONFIG	USRP_PC

#define	bmPC_nRESET		bmBIT0		// reset line to codecs (active low)
#define bmALTERA_DATA0		bmBIT1
#define bmALTERA_NCONFIG	bmBIT2
#define bmALTERA_DCLK		bmBIT3
#define bmALTERA_CONF_DONE	bmBIT4
#define bmALTERA_NSTATUS	bmBIT5
#define	bmPC_LED0		bmBIT6		// active low
#define	bmPC_LED1		bmBIT7		// active low

sbit at 0xA0+1 bitALTERA_DATA0;		// 0xA0 is the bit address of PORT C
sbit at 0xA0+3 bitALTERA_DCLK;


#define	bmALTERA_BITS		(bmALTERA_DATA0			\
				 | bmALTERA_NCONFIG		\
				 | bmALTERA_DCLK		\
				 | bmALTERA_CONF_DONE		\
				 | bmALTERA_NSTATUS)

#define	bmPORT_C_OUTPUTS	(bmPC_nRESET			\
				 | bmALTERA_DATA0 		\
				 | bmALTERA_NCONFIG		\
				 | bmALTERA_DCLK		\
				 | bmPC_LED0			\
				 | bmPC_LED1			\
				 )

#define	bmPORT_C_INITIAL	(bmPC_LED0 | bmPC_LED1)


#define	USRP_LED_REG		USRP_PC
#define	bmLED0			bmPC_LED0
#define	bmLED1			bmPC_LED1


/* Port D: GPIF	FD[15:8]		*/

/* Port E: not bit addressible		*/

#define	USRP_PE			IOE		// Port E
#define	USRP_PE_OE		OEE		// Port E direction register

#define bmPE_PE0		bmBIT0		// GPIF debug output
#define	bmPE_PE1		bmBIT1		// GPIF debug output
#define	bmPE_PE2		bmBIT2		// GPIF debug output
#define	bmPE_FPGA_CLR_STATUS	bmBIT3		// misc pin to FPGA (clear status)
#define	bmPE_SEN_TX_A		bmBIT4		// serial enable d'board TX A (active low)
#define	bmPE_SEN_RX_A		bmBIT5		// serial enable d'board RX A (active low)
#define	bmPE_SEN_TX_B		bmBIT6		// serial enable d'board TX B (active low)
#define bmPE_SEN_RX_B		bmBIT7		// serial enable d'board RX B (active low)


#define	bmPORT_E_OUTPUTS	(bmPE_FPGA_CLR_STATUS	\
				 | bmPE_SEN_TX_A 	\
				 | bmPE_SEN_RX_A	\
				 | bmPE_SEN_TX_B	\
				 | bmPE_SEN_RX_B	\
				 )


#define	bmPORT_E_INITIAL	(bmPE_SEN_TX_A 		\
				 | bmPE_SEN_RX_A	\
				 | bmPE_SEN_TX_B	\
				 | bmPE_SEN_RX_B	\
				 )

/*
 * FPGA output lines that are tied to FX2 RDYx inputs.
 * These are readable using GPIFREADYSTAT.
 */
#define	bmFPGA_HAS_SPACE		bmBIT0	// usbrdy[0] has room for 512 byte packet
#define	bmFPGA_PKT_AVAIL		bmBIT1	// usbrdy[1] has >= 512 bytes available
// #define	bmTX_UNDERRUN			bmBIT2  // usbrdy[2] D/A ran out of data
// #define	bmRX_OVERRUN			bmBIT3	// usbrdy[3] A/D ran out of buffer

/*
 * FPGA input lines that are tied to the FX2 CTLx outputs.
 *
 * These are controlled by the GPIF microprogram...
 */
// WR					bmBIT0	// usbctl[0]
// RD					bmBIT1	// usbctl[1]
// OE					bmBIT2	// usbctl[2]

#endif /* _USRP_REV1_REGS_H_ */
