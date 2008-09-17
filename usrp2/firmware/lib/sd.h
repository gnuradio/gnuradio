/* -*- c -*- */
/*
 * Copyright 2008 Ettus Research LLC
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

#ifndef INCLUDED_SD_H
#define INCLUDED_SD_H

#include "memory_map.h"

#define SD_READY 1
#define SD_IDLE_WAIT_MAX 100
#define SD_CMD_TIMEOUT 100
#define SD_RD_TIMEOUT 1000

#define SD_CMD0 0
#define SD_CMD1 1
#define SD_CMD9 9
#define SD_CMD10 10
#define SD_CMD12 12
#define SD_CMD13 13
#define SD_CMD16 16
#define SD_CMD17 17 
#define SD_CMD18 18
#define SD_CMD24 24
#define SD_CMD25 25
#define SD_CMD27 27
#define SD_CMD28 28
#define SD_CMD29 29
#define SD_CMD30 30
#define SD_CMD32 32
#define SD_CMD33 33
#define SD_CMD38 38
#define SD_CMD55 55
#define SD_CMD58 58
#define SD_CMD59 59
#define SD_ACMD41 41
#define SD_IDLE 0xFF
#define SD_CRC 0x95

#define SD_R1 1
#define SD_R1B 2
#define SD_R2 3
#define SD_R3 4

#define SD_CMD0_R SD_R1
#define SD_CMD16_R SD_R1
#define SD_CMD17_R SD_R1
#define SD_CMD55_R SD_R1
#define SD_ACMD41_R SD_R1
#define SD_CMD58_R SD_R3

#define SD_BLOCKLEN 512
#define SD_BLOCKLEN_NBITS 9

#define SD_MSK_IDLE 0x01
#define SD_MSK_OCR_33 0xC0
#define SD_MSK_TOK_DATAERROR 0xE0


int sd_init(void);

static inline void
sd_assert_cs(void)
{
  // Wait for idle before doing anything
  while(sdspi_regs->status != SD_READY)
    ;
  sdspi_regs->status = 1;
}

static inline void
sd_deassert_cs(void)
{
  // Wait for idle before doing anything
  while(sdspi_regs->status != SD_READY)
    ;
  sdspi_regs->status = 0;
}

static inline char
sd_rcv_byte(void)
{
  // Wait for idle before doing anything
  while(sdspi_regs->status != SD_READY)
    ;
  sdspi_regs->send_dat = SD_IDLE;
  while(sdspi_regs->status != SD_READY)
    ;
  return sdspi_regs-> receive_dat;
}

static inline void
sd_send_byte(char dat)
{
  // Wait for idle before doing anything
  while(sdspi_regs->status != SD_READY)
    ;      // Wait for status = 1 (ready)
  sdspi_regs->send_dat = dat;
}


int sd_send_command(unsigned char cmd,unsigned char response_type,
		    unsigned char *response,unsigned char *argument);

int sd_read_block (unsigned int blockaddr, unsigned char *buf);
int sd_write_block(unsigned int blockaddr, const unsigned char *buf);

#endif /* INCLUDED_SD_H */
