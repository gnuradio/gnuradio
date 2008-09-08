/* -*- c++ -*- */
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
#ifndef INCLUDED_DBSM_H
#define INCLUDED_DBSM_H

/*
 * Double Buffering State Machine
 */

#include <stdint.h>
#include "bool.h"

struct _dbsm;
typedef struct _dbsm dbsm_t;

/*
 * pointer to function that does packet inspection.
 *
 * If one of these returns true, it means that the s/w
 * handled that packet, and that it should NOT be passed
 * on to the normal destination port.
 */
typedef bool (*inspector_t)(dbsm_t *sm, int buf_this);

bool dbsm_nop_inspector(dbsm_t *sm, int buf_this);	// returns false


typedef struct
{
  uint16_t	port;
  uint16_t	first_line;
  uint16_t	last_line;
} buf_cmd_args_t;

/*!
 * double buffer state machine
 */
struct _dbsm
{
  uint8_t	  buf0;	     // Must be even. This machine uses buf0 and buf0+1
  uint8_t	  running;
  uint8_t	  rx_idle;
  uint8_t	  tx_idle;
  buf_cmd_args_t  recv_args;
  buf_cmd_args_t  send_args;
  inspector_t	  inspect;
  uint32_t	  precomputed_receive_to_buf_ctrl_word[2];
  uint32_t	  precomputed_send_from_buf_ctrl_word[2];
  int		  last_line_adj;
};

void dbsm_init(dbsm_t *sm, int buf0,
	       const buf_cmd_args_t *recv, const buf_cmd_args_t *send,
	       inspector_t inspect);

void dbsm_start(dbsm_t *sm);
void dbsm_stop(dbsm_t *sm);
void dbsm_process_status(dbsm_t *sm, uint32_t status);
void dbsm_handle_tx_underrun(dbsm_t *sm);
void dbsm_handle_rx_overrun(dbsm_t *sm);

/*
 * The cpu calls this when it want to ensure that it can send a buffer
 * to the same destination being used by this state machine.
 *
 * If neither buffer is EMPTYING it returns immediately.  If a buffer
 * is EMPYTING, it waits for the h/w to transition to the DONE or
 * ERROR state.
 *
 * When this function returns, the caller queues it's buffer and busy
 * waits for it to complete.
 */
void dbsm_wait_for_opening(dbsm_t *sm);

#endif /* INCLUDED_DBSM_H */
