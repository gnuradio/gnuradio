/* -*- c++ -*- */
/*
 * Copyright 2003 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
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

/*
 * USB Vendor and Product IDs that we use
 *
 * (keep in sync with usb_descriptors.a51)
 */

#ifndef _USRP_IDS_H_
#define _USRP_IDS_H_

#define	USB_VID_CYPRESS			0x04b4
#define	USB_PID_CYPRESS_FX2		0x8613


#define	USB_VID_FSF			0xfffe	  // Free Software Folks
#define	USB_PID_FSF_EXP_0		0x0000	  // Experimental 0
#define	USB_PID_FSF_EXP_1		0x0001	  // Experimental 1
#define	USB_PID_FSF_USRP		0x0002	  // Universal Software Radio Peripheral
#define	USB_PID_FSF_USRP_reserved	0x0003	  // Universal Software Radio Peripheral
#define	USB_PID_FSF_SSRP		0x0004	  // Simple Software Radio Peripheral
#define	USB_PID_FSF_SSRP_reserved	0x0005	  // Simple Software Radio Peripheral
#define USB_PID_FSF_HPSDR               0x0006    // High Performance Software Defined Radio (Internal Boot)
#define USB_PID_FSF_HPSDR_HA    	0x0007    // High Performance Software Defined Radio (Host Assisted Boot)

#define USB_PID_FSF_LBNL_UXO            0x0018    // http://recycle.lbl.gov/~ldoolitt/uxo/


#define	USB_DID_USRP_0			0x0000	  // unconfigured rev 0 USRP
#define	USB_DID_USRP_1			0x0001	  // unconfigured rev 1 USRP
#define	USB_DID_USRP_2			0x0002	  // unconfigured rev 2 USRP

#endif /* _USRP_IDS_H_ */
