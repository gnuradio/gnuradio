/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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

#ifndef INCLUDED_SPI_H
#define INCLUDED_SPI_H

#include "usrp_spi_defs.h"

void init_spi (void);		// one time call to init

// returns non-zero if successful, else 0
unsigned char
spi_read (unsigned char header_hi, unsigned char header_lo,
	  unsigned char enables, unsigned char format,
	  xdata unsigned char *buf, unsigned char len);

// returns non-zero if successful, else 0
unsigned char
spi_write (unsigned char header_hi, unsigned char header_lo,
	   unsigned char enables, unsigned char format,
	   const xdata unsigned char *buf, unsigned char len);


#endif /* INCLUDED_SPI_H */
