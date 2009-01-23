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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "bsm12.h"
#include "memory_map.h"
#include "buffer_pool.h"
#include "bool.h"
#include "nonstdio.h"
#include "buffer_state.h"
#include <stdlib.h>

static uint32_t last_send_ctrl[NBUFFERS];

void
bsm12_init(bsm12_t *sm, int buf0,
	   const buf_cmd_args_t *recv,
	   const buf_cmd_args_t *send0,
	   const buf_cmd_args_t *send1,
	   bsm12_inspector_t inspect)
{
  if (buf0 & 0x3)	// precondition: buf0 % 4 == 0
    abort();

  sm->buf0 = buf0;
  sm->running = false;
  sm->recv_args = *recv;
  sm->send_args[0] = *send0;
  sm->send_args[1] = *send1;

  sm->rx_idle = true;
  sm->tx_idle[0] = true;
  sm->tx_idle[1] = true;

  sm->inspect = inspect;

  sm->bps_error = BPS_ERROR(buf0 + 0) | BPS_ERROR(buf0 + 1) | BPS_ERROR(buf0 + 2);
  sm->bps_done  = BPS_DONE(buf0 + 0)  | BPS_DONE(buf0 + 1)  | BPS_DONE(buf0 + 2);
  sm->bps_error_or_done = sm->bps_error | sm->bps_done;

  // How much to adjust the last_line register.
  // It's 1 for everything but the ethernet.
  sm->last_line_adj = recv->port == PORT_ETH ? 3 : 1;

  buffer_state[sm->buf0 + 0] = BS_EMPTY;
  buffer_state[sm->buf0 + 1] = BS_EMPTY;
  buffer_state[sm->buf0 + 2] = BS_EMPTY;

  for (int i = 0; i < 3; i++){
    sm->precomputed_receive_to_buf_ctrl_word[i] =
      (BPC_READ
       | BPC_BUFFER(sm->buf0 + i)
       | BPC_PORT(sm->recv_args.port)
       | BPC_STEP(1)
       | BPC_FIRST_LINE(sm->recv_args.first_line)
       | BPC_LAST_LINE(sm->recv_args.last_line));
    
    for (int j = 0; j < 2; j++){
      sm->precomputed_send_from_buf_ctrl_word[i][j] =
	(BPC_WRITE
	 | BPC_BUFFER(sm->buf0 + i)
	 | BPC_PORT(sm->send_args[j].port)
	 | BPC_STEP(1)
	 | BPC_FIRST_LINE(sm->send_args[j].first_line)
	 | BPC_LAST_LINE(0));		// last line filled in at runtime
    }
  }
}

static inline void
bsm12_receive_to_buf(bsm12_t *sm, int bufno)
{
  buffer_pool_ctrl->ctrl = sm->precomputed_receive_to_buf_ctrl_word[bufno & 0x3];
}

static inline void
bsm12_send_from_buf(bsm12_t *sm, int bufno, int dst_idx)
{
  uint32_t t = (sm->precomputed_send_from_buf_ctrl_word[bufno & 0x3][dst_idx]
		| BPC_LAST_LINE(buffer_pool_status->last_line[bufno] - sm->last_line_adj));

  buffer_pool_ctrl->ctrl = t;
  last_send_ctrl[bufno] = t;
}

static inline void
bsm12_resend_from_buf(bsm12_t *sm, int bufno)
{
  buffer_pool_ctrl->ctrl = last_send_ctrl[bufno];
}

void
bsm12_start(bsm12_t *sm)
{
  sm->running = true;

  buffer_state[sm->buf0 + 0] = BS_EMPTY;
  buffer_state[sm->buf0 + 1] = BS_EMPTY;
  buffer_state[sm->buf0 + 2] = BS_EMPTY;

  bp_clear_buf(sm->buf0 + 0);
  bp_clear_buf(sm->buf0 + 1);
  bp_clear_buf(sm->buf0 + 2);

  sm->rx_idle = false;
  sm->tx_idle[0] = true;
  sm->tx_idle[1] = true;
  bsm12_receive_to_buf(sm, sm->buf0);
  buffer_state[sm->buf0] = BS_FILLING;
}

void
bsm12_stop(bsm12_t *sm)
{
  sm->running = false;
  bp_clear_buf(sm->buf0 + 0);
  bp_clear_buf(sm->buf0 + 1);
  bp_clear_buf(sm->buf0 + 2);
  buffer_state[sm->buf0 + 0] = BS_EMPTY;
  buffer_state[sm->buf0 + 1] = BS_EMPTY;
  buffer_state[sm->buf0 + 2] = BS_EMPTY;
}

static void bsm12_process_helper(bsm12_t *sm, int buf_this);
static void bsm12_error_helper(bsm12_t *sm, int buf_this);

void
bsm12_process_status(bsm12_t *sm, uint32_t status)
{
  // anything for us?
  if ((status & sm->bps_error_or_done) == 0 || !sm->running)
    return;

  if (status & sm->bps_error){
    // Most likely an ethernet Rx error.  We just restart the transfer.
    if (status & (BPS_ERROR(sm->buf0 + 0)))
      bsm12_error_helper(sm, sm->buf0 + 0);

    if (status & (BPS_ERROR(sm->buf0 + 1)))
      bsm12_error_helper(sm, sm->buf0 + 1);

    if (status & (BPS_ERROR(sm->buf0 + 2)))
      bsm12_error_helper(sm, sm->buf0 + 2);
  }

  if (status & BPS_DONE(sm->buf0 + 0))
    bsm12_process_helper(sm, sm->buf0 + 0);

  if (status & BPS_DONE(sm->buf0 + 1))
    bsm12_process_helper(sm, sm->buf0 + 1);

  if (status & BPS_DONE(sm->buf0 + 2))
    bsm12_process_helper(sm, sm->buf0 + 2);
}

static void 
bsm12_process_helper(bsm12_t *sm, int buf_this)
{
}

static void
bsm12_error_helper(bsm12_t *sm, int buf_this)
{
  bp_clear_buf(buf_this);		  // clears ERROR flag

  if (buffer_state[buf_this] == BS_FILLING){
    bsm12_receive_to_buf(sm, buf_this);	  // restart the xfer
  }
  else { // buffer was emptying
    bsm12_resend_from_buf(sm, buf_this);  // restart the xfer
  }
}


void
bsm12_handle_tx_underrun(bsm12_t *sm)
{
}

void
bsm12_handle_rx_overrun(bsm12_t *sm)
{
}
