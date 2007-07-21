/* 
 * USRP - Universal Software Radio Peripheral
 *
 * Copyright (C) 2003,2006 Free Software Foundation, Inc.
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
 * common defines and prototypes for USRP
 *
 * In comments below "TRM" refers to the EZ-USB FX2 Technical Reference Manual
 */

#ifndef _USRPCOMMON_H_
#define _USRPCOMMON_H_

#include <usrp_config.h>
#include <usrp_rev2_regs.h>
#include <syncdelay.h>

/*
 * From TRM page 15-105:
 *
 * Under certain conditions, some read and write access to the FX2
 * registers must be separated by a "synchronization delay".  The
 * delay is necessary only under the following conditions:
 *
 *   - between a write to any register in the 0xE600 - 0xE6FF range 
 *     and a write to one of the registers listed below.
 *
 *   - between a write to one of the registers listed below and a read 
 *     from any register in the 0xE600 - 0xE6FF range.
 *
 *   Registers which require a synchronization delay:
 *
 *	FIFORESET			FIFOPINPOLAR
 *	INPKTEND			EPxBCH:L
 *	EPxFIFOPFH:L			EPxAUTOINLENH:L
 *	EPxFIFOCFG			EPxGPIFFLGSEL
 *	PINFLAGSAB			PINFLAGSCD
 *	EPxFIFOIE			EPxFIFOIRQ
 *	GPIFIE				GPIFIRQ
 *	UDMACRCH:L			GPIFADRH:L
 *	GPIFTRIG			EPxGPIFTRIG
 *	OUTPKTEND			REVCTL
 *	GPIFTCB3			GPIFTCB2
 *	GPIFTCB1			GPIFTCB0
 */

#define	TRUE		1
#define	FALSE		0


void init_usrp (void);
void init_gpif (void);

void set_led_0 (unsigned char on);
void set_led_1 (unsigned char on);
void toggle_led_0 (void);
void toggle_led_1 (void);

#define la_trace(v)

#endif /* _USRPCOMMON_H_ */
