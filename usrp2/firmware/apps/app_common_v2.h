/* -*- c++ -*- */
/*
 * Copyright 2007,2008 Free Software Foundation, Inc.
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

#ifndef INCLUDED_APP_COMMON_H
#define INCLUDED_APP_COMMON_H

#include <stdbool.h>
#include "usrp2_eth_packet.h"
#include "dbsm.h"
#include "memory_map.h"
#include "hal_io.h"
#include <stddef.h>
#include <db.h>

#define CPU_TX_BUF 	7	// cpu -> eth

#define	_AL4 __attribute__((aligned (4)))

extern volatile bool link_is_up;	// eth handler sets this

// If there's a dbsm that sends to the ethernet, put it's address here
extern dbsm_t *ac_could_be_sending_to_eth;

extern int cpu_tx_buf_dest_port;

/*
 * Called when an ethernet packet is received.
 * Return true if we handled it here, otherwise
 * it'll be passed on to the DSP Tx pipe
 */
bool eth_pkt_inspector(dbsm_t *sm, int bufno);

void link_changed_callback(int speed);

void
print_tune_result(char *msg, bool tune_ok,
		  u2_fxpt_freq_t target_freq, struct tune_result *r);


void start_rx_streaming_cmd(op_start_rx_streaming_t *p);
void stop_rx_cmd(void);
void restart_streaming(void);
bool is_streaming(void);

#include "usrp2_ipv4_packet.h"
#include "usrp2_udp_packet.h"

/*!
 * \brief consolidated packet: padding + ethernet header + ip header + udp header
 */
typedef struct {
  uint16_t          padding;
  u2_eth_hdr_t      eth;
  u2_ipv4_hdr_t     ip;
  u2_udp_hdr_t      udp;
} u2_eth_ip_udp_t;

extern uint16_t host_dst_udp_port;
extern uint16_t host_src_udp_port;
extern struct in_addr host_dst_ip_addr;
extern struct in_addr host_src_ip_addr;
extern eth_mac_addr_t host_dst_mac_addr;
extern eth_mac_addr_t host_src_mac_addr;

#endif /* INCLUDED_APP_COMMON_H */
