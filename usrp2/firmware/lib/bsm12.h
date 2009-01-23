/* -*- c++ -*- */
/*
 * Copyright 2009 Free Software Foundation, Inc.
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

#ifndef INCLUDED_BSM12_H
#define INCLUDED_BSM12_H

#include "dbsm.h"
#include "memory_map.h"

/*!
 * buffer state machine: 1 input to two outputs
 *
 * Typically used to read packets from the ethernet and then after inspecting,
 * handle the packet in firmware or pass it on to 1 of the 2 buffer destinations.
 */

struct _bsm12;
typedef struct _bsm12 bsm12_t;

/*!
 * Pointer to function that does packet inspection.
 *
 * \param sm		the state machine
 * \param buf_this	the index of the buffer to inspect and/or pass on
 *
 * Returns -1, 0 or 1.  If it returns -1, it means that the s/w
 * handled that packet, and that it should NOT be passed on to one of
 * the buffer endpoints.  0 indicates the first endpoint, 1 the second endpoint.
 */
typedef int (*bsm12_inspector_t)(bsm12_t *sm, int buf_this);


/*!
 * buffer state machine: 1 input to two outputs
 */
struct _bsm12
{
  uint8_t	  buf0;	 // This machine uses buf0, buf0+1 and buf0+2.  buf0 % 4 == 0.
  uint8_t	  running;
  int8_t	  rx_state;	// -1, 0, 1, 2  which buffer we're receiving into
  int8_t	  tx_state[2];	// -1, 0, 1, 2  which buffer we're sending from
  buf_cmd_args_t  recv_args;
  buf_cmd_args_t  send_args[2];
  bsm12_inspector_t inspect;
  int		  last_line_adj;
  uint32_t	  bps_error;
  uint32_t	  bps_done;
  uint32_t	  bps_error_or_done;
  uint8_t	  next_buf[NBUFFERS];
  uint32_t	  precomputed_receive_to_buf_ctrl_word[3];
  uint32_t	  precomputed_send_from_buf_ctrl_word[4][2];  // really only 3, not 4 
                                                              //   (easier addr comp)
};

void bsm12_init(bsm12_t *sm, int buf0,
		const buf_cmd_args_t *recv,
		const buf_cmd_args_t *send0,
		const buf_cmd_args_t *send1,
		bsm12_inspector_t inspect);

void bsm12_start(bsm12_t *sm);
void bsm12_stop(bsm12_t *sm);
void bsm12_process_status(bsm12_t *sm, uint32_t status);
void bsm12_handle_tx_underrun(bsm12_t *sm);
void bsm12_handle_rx_overrun(bsm12_t *sm);


#endif /* INCLUDED_BSM12_H */
