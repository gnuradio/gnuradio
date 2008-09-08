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

#include "i2c.h"
#include "memory_map.h"
#include "stdint.h"

#define MAX_WB_DIV 4	// maximum wishbone divisor (from 100 MHz MASTER_CLK)

// prescaler divisor values for 100 kHz I2C [uses 5 * SCLK internally]

#define PRESCALER(wb_div) (((MASTER_CLK_RATE/(wb_div)) / (5 * 100000)) - 1)

static uint16_t prescaler_values[MAX_WB_DIV+1] = {
  0xffff,	// 0: can't happen
  PRESCALER(1),	// 1: 100 MHz
  PRESCALER(2), // 2:  50 MHz
  PRESCALER(3), // 3:  33.333 MHz
  PRESCALER(4), // 4:  25 MHz
};

void
i2c_init(void)
{
  i2c_regs->ctrl = 0;		// disable core
  
  // setup prescaler depending on wishbone divisor
  int wb_div = hwconfig_wishbone_divisor();
  if (wb_div > MAX_WB_DIV)
    wb_div = MAX_WB_DIV;

  i2c_regs->prescaler_lo = prescaler_values[wb_div] & 0xff;
  i2c_regs->prescaler_hi = (prescaler_values[wb_div] >> 8) & 0xff;

  i2c_regs->ctrl = I2C_CTRL_EN;	// enable core

  // FIXME interrupt driven?
}

static inline void
wait_for_xfer(void)
{
  while (i2c_regs->cmd_status & I2C_ST_TIP)	// wait for xfer to complete
    ;
}

static inline bool
wait_chk_ack(void)
{
  wait_for_xfer();

  if ((i2c_regs->cmd_status & I2C_ST_RXACK) != 0){	// target NAK'd
    return false;
  }
  return true;
}

bool 
i2c_read (unsigned char i2c_addr, unsigned char *buf, unsigned int len)
{
  if (len == 0)			// reading zero bytes always works
    return true;

  while (i2c_regs->cmd_status & I2C_ST_BUSY)
    ;

  i2c_regs->data = (i2c_addr << 1) | 1;	 // 7 bit address and read bit (1)
  // generate START and write addr
  i2c_regs->cmd_status = I2C_CMD_WR | I2C_CMD_START;
  if (!wait_chk_ack())
    goto fail;

  for (; len > 0; buf++, len--){
    i2c_regs->cmd_status = I2C_CMD_RD | (len == 1 ? (I2C_CMD_NACK | I2C_CMD_STOP) : 0);
    wait_for_xfer();
    *buf = i2c_regs->data;
  }
  return true;

 fail:
  i2c_regs->cmd_status = I2C_CMD_STOP;  // generate STOP
  return false;
}


bool 
i2c_write(unsigned char i2c_addr, const unsigned char *buf, unsigned int len)
{
  while (i2c_regs->cmd_status & I2C_ST_BUSY)
    ;

  i2c_regs->data = (i2c_addr << 1) | 0;	 // 7 bit address and write bit (0)

  // generate START and write addr (and maybe STOP)
  i2c_regs->cmd_status = I2C_CMD_WR | I2C_CMD_START | (len == 0 ? I2C_CMD_STOP : 0);  
  if (!wait_chk_ack())
    goto fail;

  for (; len > 0; buf++, len--){
    i2c_regs->data = *buf;
    i2c_regs->cmd_status = I2C_CMD_WR | (len == 1 ? I2C_CMD_STOP : 0);
    if (!wait_chk_ack())
      goto fail;
  }
  return true;

 fail:
  i2c_regs->cmd_status = I2C_CMD_STOP;  // generate STOP
  return false;
}

  
