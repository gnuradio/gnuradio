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

/*
 * Double Buffering State Machine
 */

#include "dbsm.h"
#include "memory_map.h"
#include "buffer_pool.h"
#include "bool.h"
#include "nonstdio.h"
#include <stdlib.h>

typedef enum {
  BS_EMPTY,
  BS_FILLING,
  BS_FULL,
  BS_EMPTYING,
} buffer_state_t;

static buffer_state_t buffer_state[NBUFFERS];

bool
dbsm_nop_inspector(dbsm_t *sm, int buf_this)
{
  return false;
}

void
dbsm_init(dbsm_t *sm, int buf0,
	  const buf_cmd_args_t *recv, const buf_cmd_args_t *send,
	  inspector_t inspect)
{
  if (buf0 & 0x1)	// must be even
    abort();

  sm->buf0 = buf0;
  sm->running = false;
  sm->recv_args = *recv;
  sm->send_args = *send;

  sm->rx_idle = true;
  sm->tx_idle = true;

  sm->inspect = inspect;

  // How much to adjust the last_line register.
  // It's 1 for everything but the ethernet.
  //sm->last_line_adj = recv->port == PORT_ETH ? 3 : 1;
  sm->last_line_adj = 1;

  buffer_state[sm->buf0] = BS_EMPTY;
  buffer_state[sm->buf0 ^ 1] = BS_EMPTY;

  sm->precomputed_receive_to_buf_ctrl_word[0] =
    (BPC_READ
     | BPC_BUFFER(sm->buf0)
     | BPC_PORT(sm->recv_args.port)
     | BPC_STEP(1)
     | BPC_FIRST_LINE(sm->recv_args.first_line)
     | BPC_LAST_LINE(sm->recv_args.last_line));
    
  sm->precomputed_receive_to_buf_ctrl_word[1] =
    (BPC_READ
     | BPC_BUFFER(sm->buf0 ^ 1)
     | BPC_PORT(sm->recv_args.port)
     | BPC_STEP(1)
     | BPC_FIRST_LINE(sm->recv_args.first_line)
     | BPC_LAST_LINE(sm->recv_args.last_line));
    
  sm->precomputed_send_from_buf_ctrl_word[0] =
    (BPC_WRITE
     | BPC_BUFFER(sm->buf0)
     | BPC_PORT(sm->send_args.port)
     | BPC_STEP(1)
     | BPC_FIRST_LINE(sm->send_args.first_line)
     | BPC_LAST_LINE(0));		// last line filled in at runtime
    
  sm->precomputed_send_from_buf_ctrl_word[1] =
    (BPC_WRITE
     | BPC_BUFFER(sm->buf0 ^ 1)
     | BPC_PORT(sm->send_args.port)
     | BPC_STEP(1)
     | BPC_FIRST_LINE(sm->send_args.first_line)
     | BPC_LAST_LINE(0));		// last line filled in at runtime
    
}

static inline void
dbsm_receive_to_buf(dbsm_t *sm, int bufno)
{
  buffer_pool_ctrl->ctrl = sm->precomputed_receive_to_buf_ctrl_word[bufno & 1];
}

static inline void
dbsm_send_from_buf(dbsm_t *sm, int bufno)
{
  buffer_pool_ctrl->ctrl =
    (sm->precomputed_send_from_buf_ctrl_word[bufno & 1]
     | BPC_LAST_LINE(buffer_pool_status->last_line[bufno] - sm->last_line_adj));
}

void
dbsm_start(dbsm_t *sm)
{
  // printf("dbsm_start: buf0 = %d, recv_port = %d\n", sm->buf0, sm->recv_args.port);

  sm->running = true;

  buffer_state[sm->buf0] = BS_EMPTY;
  buffer_state[sm->buf0 ^ 1] = BS_EMPTY;

  bp_clear_buf(sm->buf0);
  bp_clear_buf(sm->buf0 ^ 1);

  sm->tx_idle = true;
  sm->rx_idle = false;
  dbsm_receive_to_buf(sm, sm->buf0);
  buffer_state[sm->buf0] = BS_FILLING;

}


void
dbsm_stop(dbsm_t *sm)
{
  sm->running = false;
  bp_clear_buf(sm->buf0);
  bp_clear_buf(sm->buf0 ^ 1);
  buffer_state[sm->buf0] = BS_EMPTY;
  buffer_state[sm->buf0 ^ 1] = BS_EMPTY;
}

static void dbsm_process_helper(dbsm_t *sm, int buf_this);
static void dbsm_error_helper(dbsm_t *sm, int buf_this);

void
dbsm_process_status(dbsm_t *sm, uint32_t status)
{
  if (!sm->running)
    return;

  if (status & (BPS_ERROR(sm->buf0) | BPS_ERROR(sm->buf0 ^ 1))){
    putchar('E');
    // Most likely an ethernet Rx error.  We just restart the transfer.
    if (status & (BPS_ERROR(sm->buf0)))
      dbsm_error_helper(sm, sm->buf0);

    if (status & (BPS_ERROR(sm->buf0 ^ 1)))
      dbsm_error_helper(sm, sm->buf0 ^ 1);
  }

  if (status & BPS_DONE(sm->buf0))
    dbsm_process_helper(sm, sm->buf0);

  if (status & BPS_DONE(sm->buf0 ^ 1))
    dbsm_process_helper(sm, sm->buf0 ^ 1);
}

static void
dbsm_process_helper(dbsm_t *sm, int buf_this)
{
  int buf_other = buf_this ^ 1;

  bp_clear_buf(buf_this);

  if (buffer_state[buf_this] == BS_FILLING){
    buffer_state[buf_this] = BS_FULL;
    //
    // does s/w handle this packet?
    //
    if (sm->inspect(sm, buf_this)){
      // s/w handled the packet; refill the buffer
      dbsm_receive_to_buf(sm, buf_this);
      buffer_state[buf_this] = BS_FILLING;
    }

    else {	// s/w didn't handle this; pass it on

      if(buffer_state[buf_other] == BS_EMPTY){
	dbsm_receive_to_buf(sm, buf_other);
	buffer_state[buf_other] = BS_FILLING;
      }
      else
	sm->rx_idle = true;

      if (sm->tx_idle){
	sm->tx_idle = false;
	dbsm_send_from_buf(sm, buf_this);
	buffer_state[buf_this] = BS_EMPTYING;
      }
    }
  }
  else {  // buffer was emptying
    buffer_state[buf_this] = BS_EMPTY;
    if (sm->rx_idle){
      sm->rx_idle = false;
      dbsm_receive_to_buf(sm, buf_this);
      buffer_state[buf_this] = BS_FILLING;
    }
    if (buffer_state[buf_other] == BS_FULL){
      dbsm_send_from_buf(sm, buf_other);
      buffer_state[buf_other] = BS_EMPTYING;
    }
    else
      sm->tx_idle = true;
  }
}

static void
dbsm_error_helper(dbsm_t *sm, int buf_this)
{
  bp_clear_buf(buf_this);		// clears ERROR flag

  if (buffer_state[buf_this] == BS_FILLING){
    dbsm_receive_to_buf(sm, buf_this);	  // restart the xfer
  }
  else { // buffer was emptying
    dbsm_send_from_buf(sm, buf_this);	  // restart the xfer
  }
}

/*
 * Handle DSP Tx underrun
 */
void
dbsm_handle_tx_underrun(dbsm_t *sm)
{
  // clear the DSP Tx state machine
  dsp_tx_regs->clear_state = 1;

  // If there's a buffer that's empyting, clear it & flush xfer

  if (buffer_state[sm->buf0] == BS_EMPTYING){
    bp_clear_buf(sm->buf0);
    dsp_tx_regs->clear_state = 1;	// flush partial packet
    // drop frame in progress on ground.  Pretend it finished
    dbsm_process_helper(sm, sm->buf0);
  }
  else if (buffer_state[sm->buf0 ^ 1] == BS_EMPTYING){
    bp_clear_buf(sm->buf0 ^ 1);
    dsp_tx_regs->clear_state = 1;	// flush partial packet
    // drop frame in progress on ground.  Pretend it finished
    dbsm_process_helper(sm, sm->buf0 ^ 1);
  }
}

/*
 * Handle DSP Rx overrun
 */
void
dbsm_handle_rx_overrun(dbsm_t *sm)
{
  dsp_rx_regs->clear_state = 1;

  // If there's a buffer that's filling, clear it.
  // Any restart will be the job of the caller.
  
  if (buffer_state[sm->buf0] == BS_FILLING)
    bp_clear_buf(sm->buf0);

  if (buffer_state[sm->buf0 ^1] == BS_FILLING)
    bp_clear_buf(sm->buf0 ^ 1);
}

void 
dbsm_wait_for_opening(dbsm_t *sm)
{
  if (buffer_state[sm->buf0] == BS_EMPTYING){
    // wait for xfer to complete
    int mask = BPS_DONE(sm->buf0) | BPS_ERROR(sm->buf0) | BPS_IDLE(sm->buf0);
    while ((buffer_pool_status->status & mask) == 0)
      ;
  }
  else if (buffer_state[sm->buf0 ^ 1] == BS_EMPTYING){
    // wait for xfer to complete
    int mask = BPS_DONE(sm->buf0 ^ 1) | BPS_ERROR(sm->buf0 ^ 1) | BPS_IDLE(sm->buf0 ^ 1);
    while ((buffer_pool_status->status & mask) == 0)
      ;
  }
}
