/*
 * Copyright 2010 Free Software Foundation, Inc.
 *
 * Copyright 2010 Ettus Research LLC
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
 *
 */

#ifndef ADF4350_REGS_H
#define ADF4350_REGS_H

#include <db_base.h>
#include <stdint.h>

void adf4350_load_register(uint8_t addr, struct db_base *dbb);

#endif /* ADF4350_REGS_H */
