/* -*- c++ -*- */
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
#ifndef INCLUDED_AD9777_H
#define INCLUDED_AD9777_H

#include <stdint.h>
#include "ad9777_regs.h"

/*
 * Analog Devices AD9777 16-bit, 160 MS/s, Dual Interpolating TxDAC
 */

void ad9777_write_reg(int regno, uint8_t value);
int  ad9777_read_reg(int regno);

#endif /* INCLUDED_AD9777_H */
