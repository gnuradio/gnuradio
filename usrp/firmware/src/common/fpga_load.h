/* 
 * USRP - Universal Software Radio Peripheral
 *
 * Copyright (C) 2003 Free Software Foundation, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Boston, MA  02110-1301  USA
 */

#ifndef INCLUDED_FPGA_LOAD_H
#define INCLUDED_FPGA_LOAD_H

unsigned char fpga_load_begin (void);
unsigned char fpga_load_xfer (xdata unsigned char *p, unsigned char len);
unsigned char fpga_load_end (void);

#endif /* INCLUDED_FPGA_LOAD_H */
