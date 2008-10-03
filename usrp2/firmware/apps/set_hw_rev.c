/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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
#include <u2_init.h>
#include <nonstdio.h>
#include <i2c.h>
#include <usrp2_i2c_addr.h>

#define HW_REV_MAJOR 0
#define HW_REV_MINOR 3

int
main(void)
{
  u2_init();

  putstr("\nset_hw_rev\n");

  bool ok = true;
  unsigned char maj = HW_REV_MAJOR;
  unsigned char min = HW_REV_MINOR;
  ok = eeprom_write(I2C_ADDR_MBOARD, MBOARD_REV_MSB, &maj, 1);
  ok &= eeprom_write(I2C_ADDR_MBOARD, MBOARD_REV_LSB, &min, 1);

  if (ok)
    printf("OK: set h/w rev to %d.%d\n", HW_REV_MAJOR, HW_REV_MINOR);
  else
    printf("FAILED to set h/w rev to %d.%d\n", HW_REV_MAJOR, HW_REV_MINOR);

  return 0;
}
