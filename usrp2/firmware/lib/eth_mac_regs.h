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
 * See opencores.org 10_100_1000 Mbps Tri-mode Ethernet MAC Specification
 *
 * In reality, these are 16-bit regs, but are assigned
 * on 32-bit boundaries.  Because we're little endian,
 * declaring them "int" works.
 */
typedef struct {
  volatile int	tx_hwmark;
  volatile int	tx_lwmark;

  //! if set, send pause frames automatically
  volatile int	pause_frame_send_en;

  //! quanta value for pause frame in units of 512 bit times.
  volatile int	pause_quanta_set;

  volatile int	ifg_set;
  volatile int	full_duplex;
  volatile int	max_retry;
  volatile int	mac_tx_add_en;
  volatile int	mac_tx_add_prom_data;
  volatile int	mac_tx_add_prom_add;
  volatile int	mac_tx_add_prom_wr;

  //! if set, other end can pause us (i.e., we pay attention to pause frames)
  volatile int	tx_pause_en;

  // Flow Control high and low water marks
  //! when space available (in 32-bit lines) > hwmark, send un-pause frame
  volatile int	fc_hwmark;	

  //! when space avail (in 32-bit lines) < lwmark, send pause frame
  volatile int	fc_lwmark;	

  volatile int	mac_rx_add_chk_en;
  volatile int	mac_rx_add_prom_data;
  volatile int	mac_rx_add_prom_add;
  volatile int	mac_rx_add_prom_wr;
  volatile int	broadcast_filter_en;
  volatile int	broadcast_bucket_depth;
  volatile int	broadcast_bucket_interval;
  volatile int	rx_append_crc;
  volatile int	rx_hwmark;
  volatile int	rx_lwmark;
  volatile int	crc_chk_en;
  volatile int	rx_ifg_set;
  volatile int	rx_max_length;
  volatile int	rx_min_length;
  volatile int	rmon_rd_addr;		// performance counter access
  volatile int	rmon_rd_apply;		
  volatile int	rmon_rd_grant;		// READONLY
  volatile int	rmon_rd_dout;	        // READONLY
  volatile int	dummy;	// READONLY
  volatile int	line_loop_en;
  volatile int	speed;
  volatile int	miimoder;
  volatile int	miicommand;
  volatile int	miiaddress;
  volatile int	miitx_data;
  volatile int	miirx_data;
  volatile int	miistatus;
} eth_mac_regs_t;

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
