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

#ifndef INCLUDED_I2C_H
#define INCLUDED_I2C_H

#include "bool.h"

void i2c_init(void);
bool i2c_read (unsigned char i2c_addr, unsigned char *buf, unsigned int len);
bool i2c_write(unsigned char i2c_addr, const unsigned char *buf, unsigned int len);


// Write 24LC024 / 24LC025 EEPROM on motherboard or daughterboard.
// Which EEPROM is determined by i2c_addr.  See i2c_addr.h

bool eeprom_write (int i2c_addr, int eeprom_offset, const void *buf, int len);

// Read 24LC024 / 24LC025 EEPROM on motherboard or daughterboard.
// Which EEPROM is determined by i2c_addr.  See i2c_addr.h

bool eeprom_read (int i2c_addr, int eeprom_offset, void *buf, int len);

#endif /* INCLUDED_I2C_H */
