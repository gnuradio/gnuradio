/* -*- c++ -*- */
/*
 * Copyright 2003 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include "i2c.h"
#include "fx2regs.h"
#include <string.h>


// issue a stop bus cycle and wait for completion


// returns non-zero if successful, else 0
unsigned char
i2c_read (unsigned char addr, xdata unsigned char *buf, unsigned char len)
{
  volatile unsigned char	junk;
  
  if (len == 0)			// reading zero bytes always works
    return 1;
  
  // memset (buf, 0, len);		// FIXME, remove

  while (I2CS & bmSTOP)		// wait for stop to clear
    ;


  I2CS = bmSTART;
  I2DAT = (addr << 1) | 1;	// write address and direction (1's the read bit)

  while ((I2CS & bmDONE) == 0)
    ;

  if ((I2CS & bmBERR) || (I2CS & bmACK) == 0)	// no device answered...
    goto fail;

  if (len == 1)
    I2CS |= bmLASTRD;		

  junk = I2DAT;			// trigger the first read cycle

#if 1
  while (len != 1){
    while ((I2CS & bmDONE) == 0)
      ;

    if (I2CS & bmBERR)
      goto fail;

    len--;
    if (len == 1)
      I2CS |= bmLASTRD;
    
    *buf++ = I2DAT;		// get data, trigger another read
  }
#endif

  // wait for final byte
  
  while ((I2CS & bmDONE) == 0)
    ;
  
  if (I2CS & bmBERR)
    goto fail;

  I2CS |= bmSTOP;
  *buf = I2DAT;

  return 1;

 fail:
  I2CS |= bmSTOP;
  return 0;
}



// returns non-zero if successful, else 0
unsigned char
i2c_write (unsigned char addr, xdata const unsigned char *buf, unsigned char len)
{
  while (I2CS & bmSTOP)		// wait for stop to clear
    ;

  I2CS = bmSTART;
  I2DAT = (addr << 1) | 0;	// write address and direction (0's the write bit)

  while ((I2CS & bmDONE) == 0)
    ;

  if ((I2CS & bmBERR) || (I2CS & bmACK) == 0)	// no device answered...
    goto fail;

  while (len > 0){
    I2DAT = *buf++;
    len--;

    while ((I2CS & bmDONE) == 0)
      ;

    if ((I2CS & bmBERR) || (I2CS & bmACK) == 0)	// no device answered...
      goto fail;
  }

  I2CS |= bmSTOP;
  return 1;

 fail:
  I2CS |= bmSTOP;
  return 0;
}
