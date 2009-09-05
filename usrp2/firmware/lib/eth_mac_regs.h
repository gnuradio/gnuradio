/* -*- c -*- */
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

#ifndef INCLUDED_ETH_MAC_REGS_H
#define INCLUDED_ETH_MAC_REGS_H

/*
 * Simple GEMAC
 *
 */
typedef struct {
  volatile int settings;
  volatile int ucast_hi;
  volatile int ucast_lo;
  volatile int mcast_hi;
  volatile int mcast_lo;
  volatile int miimoder;
  volatile int miiaddress;
  volatile int miitx_data;
  volatile int miicommand;
  volatile int miistatus;
  volatile int miirx_data;
  volatile int pause_time;
  volatile int pause_thresh;
} eth_mac_regs_t;

// settings register
#define MAC_SET_PAUSE_EN  (1 << 0)   // Makes us respect received pause frames (normally on)
#define MAC_SET_PASS_ALL  (1 << 1)   // Enables promiscuous mode, currently broken
#define MAC_SET_PASS_PAUSE (1 << 2)  // Sends pause frames through (normally off)
#define MAC_SET_PASS_BCAST (1 << 3)  // Sends broadcast frames through (normally on)
#define MAC_SET_PASS_MCAST (1 << 4)  // Sends multicast frames that match mcast addr (normally off)
#define MAC_SET_PASS_UCAST (1 << 5)  // Sends unicast (normal) frames through if they hit in address filter (normally on)
#define MAC_SET_PAUSE_SEND_EN (1 << 6) // Enables sending pause frames

// miicommand register
#define MIIC_SCANSSTAT	(1 << 0)	// Scan status
#define MIIC_RSTAT      (1 << 1)	// Read status
#define	MIIC_WCTRLDATA	(1 << 2)	// Write control data

// miistatus register
#define MIIS_LINKFAIL	(1 << 0)	// The link failed
#define	MIIS_BUSY	(1 << 1)	// The MII is busy (operation in progress)
#define	MIIS_NVALID	(1 << 2)	// The data in the status register is invalid
					//   This it is only valid when the scan status is active.

#endif /* INCLUDED_ETH_MAC_REGS_H */
