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
 * Loopback SERDES to SERDES
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "u2_init.h"
#include "memory_map.h"
#include "spi.h"
#include "hal_io.h"
#include "buffer_pool.h"
#include "pic.h"
#include "bool.h"
#include "nonstdio.h"
#include "memset_wa.h"
#include <dbsm.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <clocks.h>



// ----------------------------------------------------------------

#define SERDES_RX_BUF_0		0
#define	SERDES_RX_BUF_1		1

/*
 * ================================================================
 *      configure SD RX double buffering state machine
 * ================================================================
 */

// receive from SERDES
buf_cmd_args_t sd_recv_args = {
  PORT_SERDES,
  0,
  BP_LAST_LINE
};

// send to SERDES
buf_cmd_args_t sd_send_args = {
  PORT_SERDES,
  0,		// starts with packet in line 0
  0,		// filled in from list_line register
};

dbsm_t sd_sm;	// the state machine




// ----------------------------------------------------------------

#if 0
static bool
check_packet(int *buf, int nlines)
{
  bool ok = true;
  int i = 0;
  for (i = 0; i < nlines; i++){
    int expected = ((2*i + 0) << 16) | (2*i+1);
    if (buf[i] != expected){
      ok = false;
      printf("buf[%d] = 0x%x  expected = 0x%x\n", i, buf[i], expected);
    }
  }
  return ok;
}

static void
zero_buffer(int bufno)
{
  memset_wa(buffer_ram(bufno), 0, BP_NLINES * 4);
}
#endif


bool
sd_rx_inspector(dbsm_t *sm, int buf_this)
{
  hal_toggle_leds(0x2);

#if 0
  int last_line = buffer_pool_status->last_line[buf_this];
  bool ok = check_packet(buffer_ram(buf_this), last_line);
  static int good = 0;
  static int bad = 0;

  if (ok)
    good++;
  else
    bad++;

  if(good+bad == 10000) {
    printf("Good %d\tBad %d\n",good,bad);
    good = 0;
    bad = 0;
  }
#endif

  return false;
}


inline static void
buffer_irq_handler(void)
{
  uint32_t  status = buffer_pool_status->status;
  dbsm_process_status(&sd_sm, status);
}


int
main(void)
{
  u2_init();

  putstr("\nsd_bounce\n");

  // Get our clock from the mimo interface
  clocks_mimo_config(MC_WE_LOCK_TO_MIMO);

  dbsm_init(&sd_sm, SERDES_RX_BUF_0,
	    &sd_recv_args, &sd_send_args,
	    sd_rx_inspector);

  // kick off the state machine
  dbsm_start(&sd_sm);

  while(1){
    buffer_irq_handler();
  }
}
