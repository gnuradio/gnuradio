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
#include "mdelay.h"

static const int EEPROM_PAGESIZE = 16;

bool
eeprom_write (int i2c_addr, int eeprom_offset, const void *buf, int len)
{
  unsigned char cmd[2];
  const unsigned char *p = (unsigned char *) buf;
  
  // The simplest thing that could possibly work:
  //   all writes are single byte writes.
  //
  // We could speed this up using the page write feature,
  // but we write so infrequently, why bother...

  while (len-- > 0){
    cmd[0] = eeprom_offset++;
    cmd[1] = *p++;
    bool r = i2c_write (i2c_addr, cmd, sizeof (cmd));
    mdelay (10);	// delay 10ms worst case write time
    if (!r)
      return false;
  }
  return true;
}

bool
eeprom_read (int i2c_addr, int eeprom_offset, void *buf, int len)
{
  unsigned char *p = (unsigned char *) buf;

  // We setup a random read by first doing a "zero byte write".
  // Writes carry an address.  Reads use an implicit address.

  unsigned char cmd[1];
  cmd[0] = eeprom_offset;
  if (!i2c_write (i2c_addr, cmd, sizeof (cmd)))
    return false;

  while (len > 0){
    // int n = std::min (len, MAX_EP0_PKTSIZE);
    int n = len;
    if (!i2c_read (i2c_addr, p, n))
      return false;
    len -= n;
    p += n;
  }
  return true;
}
 
