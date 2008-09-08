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

#include "memory_map.h"
#include "buffer_pool.h"
#include "hal_io.h" 

void
bp_init(void)
{
  int i;
  bp_disable_port(PORT_SERDES);
  bp_disable_port(PORT_DSP);
  bp_disable_port(PORT_ETH);
  bp_disable_port(PORT_RAM);

  for (i = 0; i < NBUFFERS; i++)
    bp_clear_buf(i);
}

#ifndef INLINE_BUFFER_POOL

void
bp_clear_buf(int bufnum)
{
  buffer_pool_ctrl->ctrl = BPC_BUFFER(bufnum) | BPC_PORT_NIL | BPC_CLR;
}

void
bp_disable_port(int portnum) 
{
  // disable buffer connections to this port
  buffer_pool_ctrl->ctrl = BPC_BUFFER_NIL | BPC_PORT(portnum);
}

void
bp_receive_to_buf(int bufnum, int port, int step, int fl, int ll)
{
  buffer_pool_ctrl->ctrl = (BPC_READ
			    | BPC_BUFFER(bufnum)
			    | BPC_PORT(port)
			    | BPC_STEP(step)
			    | BPC_FIRST_LINE(fl)
			    | BPC_LAST_LINE(ll));
}

void
bp_send_from_buf(int bufnum, int port, int step, int fl, int ll)
{
  buffer_pool_ctrl->ctrl = (BPC_WRITE
			    | BPC_BUFFER(bufnum)
			    | BPC_PORT(port)
			    | BPC_STEP(step)
			    | BPC_FIRST_LINE(fl)
			    | BPC_LAST_LINE(ll));
}

#endif
