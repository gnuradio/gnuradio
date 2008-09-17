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

#include "sd.h"
#include "memory_map.h"
#include "stdint.h"
#include "stdio.h"

static inline void
sd_packarg(unsigned char *argument,unsigned int value)
{
  argument[3] = (unsigned char)(value >> 24);
  argument[2] = (unsigned char)(value >> 16);
  argument[1] = (unsigned char)(value >> 8);
  argument[0] = (unsigned char)(value);
}

int
sd_init(void)
{
  unsigned char response[5];
  unsigned char argument[4];
  int i,j;

  for(i=0;i<4;i++)
    argument[i] = 0;

  // Set clock to less than 400 kHz to start out
  sdspi_regs->clkdiv = 128;

  // Delay at least 74 cycles
  sd_assert_cs();
  for(i = 0; i < 100; i++)
    sd_send_byte(SD_IDLE);
  sd_deassert_cs();
  
  // Initialization Sequence -- CMD0 CMD55 ACMD41 CMD58
  // Put card in idle state
  if(sd_send_command(SD_CMD0,SD_CMD0_R,response,argument)==0)
    return 0;  // Something went wrong in command

  j = 0;
  do {
    j++;
    if(sd_send_command(SD_CMD55,SD_CMD55_R,response,argument)==1)
      sd_send_command(SD_ACMD41,SD_ACMD41_R,response,argument);
    else
      j = SD_IDLE_WAIT_MAX;
  }
  while(((response[0] & SD_MSK_IDLE) == SD_MSK_IDLE) && (j < SD_IDLE_WAIT_MAX));

  if(j>= SD_IDLE_WAIT_MAX)  // IDLE timeout exceeded, card asleep
    return 0;
  
  // CMD58 reads the SD card capabilities
  if(sd_send_command(SD_CMD58,SD_CMD58_R,response,argument)==0)
    return 0;  // CMD58 FAIL

  if((response[2] & SD_MSK_OCR_33) != SD_MSK_OCR_33)
    return 0;  // Card doesn't do 3.3V

  //printf("OCR = %x %x %x %x\n",response[0],response[1],response[2],response[3]);

  // Set blocklen here
  sd_packarg(argument,SD_BLOCKLEN);
  if(sd_send_command(SD_CMD16,SD_CMD16_R,response,argument)==0)
    return 0;    // Set Blocklen failed
  
  // Reset back to high speed
  sdspi_regs->clkdiv = 4;
  //puts("finished init\n");
  return 1;
}

int sd_send_command(unsigned char cmd,unsigned char response_type,
		    unsigned char *response,unsigned char *argument)
{
  int i;
  char response_length;
  unsigned char tmp;

  sd_assert_cs();
  sd_send_byte((cmd & 0x3F) | 0x40);
  for(i=3;i>=0;i--)
    sd_send_byte(argument[i]);
  sd_send_byte(SD_CRC);   // Always the same

  response_length = 0;
  switch(response_type)
    {
    case SD_R1:
    case SD_R1B:
      response_length = 1;
      break;
    case SD_R2:
      response_length = 2;
      break;
    case SD_R3:
      response_length = 5;
      break;
    default:
      break;
    }

  // Wait for a response, which will have a 0 start bit
  i = 0;
  do
    {
      tmp = sd_rcv_byte();
      i++;
    }
  while(((tmp & 0x80) != 0) && i < SD_CMD_TIMEOUT);

  if(i>= SD_CMD_TIMEOUT)
    {
      sd_deassert_cs();
      //puts("cmd send timeout\n");
      return 0;
    }

  for(i=response_length-1; i>=0; i--)
    {
      response[i] = tmp;
      tmp = sd_rcv_byte();
    }
  i = 0;
  if(response_type == SD_R1B)
    {
      do
	{
	  i++;
	  tmp = sd_rcv_byte();
	}
      while(tmp != SD_IDLE);
      sd_send_byte(SD_IDLE);
    }
  
  //puts("send cmd success\n");
  sd_deassert_cs();
  return 1;
}

int
sd_read_block (unsigned int blockaddr, unsigned char *buf)
{
  unsigned char response[5];
  unsigned char argument[4];
  unsigned int i = 0;
  unsigned char tmp;

  blockaddr <<= SD_BLOCKLEN_NBITS;
  sd_packarg(argument,blockaddr);
  if(sd_send_command(SD_CMD17,SD_CMD17_R,response,argument)==0)
    return 0;    //Failed READ;
  if(response[0] != 0)
    return 0;    //Misaligned READ

  sd_assert_cs();
  i = 0;
  do
    {
      tmp = sd_rcv_byte();
      i++;
    }
  while((tmp == 0xFF) && (i < SD_RD_TIMEOUT));
  if((i>= SD_RD_TIMEOUT) ||((tmp & SD_MSK_TOK_DATAERROR) == 0))
    {
      sd_send_byte(SD_IDLE);  // Send a dummy before quitting
      return 0;   // Data ERROR
    }
  for(i=0;i<SD_BLOCKLEN;i++)
    buf[i] = sd_rcv_byte();
  return 1;

}

int
sd_write_block(unsigned int blockaddr, const unsigned char *buf)
{
  // FIXME not implemented yet
  return 0;
}
