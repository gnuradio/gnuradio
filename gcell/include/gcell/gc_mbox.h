/* -*- c++ -*- */
/*
 * Copyright 2007,2008,2009 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef INCLUDED_GCELL_GC_MBOX_H
#define INCLUDED_GCELL_GC_MBOX_H

/*
 * The PPE and SPE exchange a few 32-bit messages via mailboxes.
 * All have a 4 bit opcode in the high bits.
 *
 *      3                   2                   1
 *    1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |  op   |                        arg                            |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 */

#define MK_MBOX_MSG(cmd, args) ((((cmd) & 0xf) << 28) | ((args) & 0x0fffffff))
#define MBOX_MSG_OP(msg)  (((msg) >> 28) & 0xf)
#define	MBOX_MSG_ARG(msg) ((msg) & 0x0fffffff)

// PPE to SPE (sent via SPE Read Inbound Mailbox)

#define OP_EXIT			0x0	// exit now
#define	OP_GET_SPU_BUFSIZE	0x1 
#define	OP_CHECK_QUEUE		0x2

// SPE to PPE (sent via SPE Write Outbound Interrupt Mailbox)

#define OP_JOBS_DONE		0x3	// arg is 0 or 1, indicating which
					//   gc_completion_info_t contains the info
#define	OP_SPU_BUFSIZE	        0x4	// arg is max number of bytes


#endif /* INCLUDED_GCELL_GC_MBOX_H */
