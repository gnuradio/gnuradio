/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
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

#include "eeprom_io.h"
#include "i2c.h"
#include "delay.h"

// returns non-zero if successful, else 0
unsigned char
eeprom_read (unsigned char i2c_addr, unsigned char eeprom_offset,
	     xdata unsigned char *buf, unsigned char len)
{
  // We setup a random read by first doing a "zero byte write".
  // Writes carry an address.  Reads use an implicit address.

  static xdata unsigned char cmd[1];
  cmd[0] = eeprom_offset;
  if (!i2c_write(i2c_addr, cmd, 1))
    return 0;

  return i2c_read(i2c_addr, buf, len);
}


#if 0

// returns non-zero if successful, else 0
unsigned char
eeprom_write (unsigned char i2c_addr, unsigned char eeprom_offset,
	      const xdata unsigned char *buf, unsigned char len)
{
  static xdata unsigned char cmd[2];
  unsigned char ok;

  while (len-- > 0){
    cmd[0] = eeprom_offset++;
    cmd[1] = *buf++;
    ok = i2c_write(i2c_addr, cmd, 2);
    mdelay(10);		// delay 10ms worst case write time
    if (!ok)
      return 0;
  }
  return 1;
}

#endif
