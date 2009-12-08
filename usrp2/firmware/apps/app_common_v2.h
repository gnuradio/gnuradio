/* -*- c++ -*- */
/*
 * Copyright 2007,2008,2009 Free Software Foundation, Inc.
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

#include "bool.h"
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

void set_reply_hdr(u2_eth_packet_t *reply_pkt, u2_eth_packet_t const *cmd_pkt);

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


void start_rx_streaming_cmd(const u2_mac_addr_t *host, op_start_rx_streaming_t *p);
void start_rx_streaming_at_cmd(const u2_mac_addr_t *host, op_start_rx_streaming_t *p, uint32_t time);
void stop_rx_cmd(void);
void restart_streaming(void);
void restart_streaming_at(uint32_t time);
bool is_streaming(void);

void handle_control_chan_frame(u2_eth_packet_t *pkt, size_t len);

#endif /* INCLUDED_APP_COMMON_H */
