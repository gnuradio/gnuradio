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

// Standard USB requests.
// These are contained in end point 0 setup packets


#ifndef _USB_REQUESTS_H_
#define _USB_REQUESTS_H_

// format of bmRequestType byte

#define	bmRT_DIR_MASK		(0x1 << 7)
#define	bmRT_DIR_IN		(1 << 7)
#define	bmRT_DIR_OUT		(0 << 7)

#define	bmRT_TYPE_MASK		(0x3 << 5)
#define	bmRT_TYPE_STD		(0 << 5)
#define	bmRT_TYPE_CLASS		(1 << 5)
#define	bmRT_TYPE_VENDOR	(2 << 5)
#define	bmRT_TYPE_RESERVED	(3 << 5)

#define	bmRT_RECIP_MASK		(0x1f << 0)
#define	bmRT_RECIP_DEVICE	(0 << 0)
#define	bmRT_RECIP_INTERFACE	(1 << 0)
#define	bmRT_RECIP_ENDPOINT	(2 << 0)
#define	bmRT_RECIP_OTHER	(3 << 0)


// standard request codes (bRequest)

#define	RQ_GET_STATUS		0
#define	RQ_CLEAR_FEATURE	1
#define	RQ_RESERVED_2		2
#define	RQ_SET_FEATURE		3
#define	RQ_RESERVED_4		4
#define	RQ_SET_ADDRESS		5
#define	RQ_GET_DESCR		6
#define	RQ_SET_DESCR		7
#define	RQ_GET_CONFIG		8
#define	RQ_SET_CONFIG		9
#define	RQ_GET_INTERFACE       10
#define	RQ_SET_INTERFACE       11
#define	RQ_SYNCH_FRAME	       12

// standard descriptor types

#define	DT_DEVICE		1
#define	DT_CONFIG		2
#define	DT_STRING		3
#define	DT_INTERFACE		4
#define	DT_ENDPOINT		5
#define	DT_DEVQUAL		6
#define	DT_OTHER_SPEED		7
#define	DT_INTERFACE_POWER	8

// standard feature selectors

#define	FS_ENDPOINT_HALT	0	// recip: endpoint
#define	FS_DEV_REMOTE_WAKEUP	1	// recip: device
#define	FS_TEST_MODE		2	// recip: device

// Get Status device attributes

#define	bmGSDA_SELF_POWERED	0x01
#define	bmGSDA_REM_WAKEUP	0x02


#endif /* _USB_REQUESTS_H_ */
