/* -*- c++ -*- */
/*
 * Copyright 2001,2004 Free Software Foundation, Inc.
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

#include "i2c_bitbang.h"

i2c_bitbang::i2c_bitbang (i2c_bbio_sptr io)
{
  d_io = io;
  d_io->lock ();

  stop ();	// get bus in known state

  d_io->unlock ();
}

i2c_sptr 
make_i2c_bitbang (i2c_bbio_sptr io)
{
  return i2c_sptr (new i2c_bitbang (io));
}


// start:
//	entry: SCL = 1, SDA = 1
//	exit:  SCL = 0, SDA = 0

void 
i2c_bitbang::start ()
{
  set_sda (1);
  set_scl (1);
  set_sda (0);		// SDA high -> low while SCL high
  set_scl (0);
}


// stop: 
//	entry: SCL = X, SDA = X
//	exit:  SCL = 1, SDA = 1

void 
i2c_bitbang::stop ()
{
  set_scl (0);
  set_sda (0);
  set_scl (1);
  set_sda (1);		// SDA low -> high while SCL high
}


// write_bit:
//	entry: SCL = 0, SDA = X
//	exit:  SCL = 0, SDA = X

void 
i2c_bitbang::write_bit (bool bit)
{
  set_sda (bit);
  set_scl (1);
  set_scl (0);
}


// write_byte:
//	entry: SCL = 0, SDA = X
//	exit:  SCL = 0, SDA = 1

bool
i2c_bitbang::write_byte (char t)
{
  int	i;
  bool	ack_bit;

  for (i = 0; i < 8; i++){
    write_bit (t & 0x80);
    t <<= 1;
  }

  // clock #9. This is the ACK bit.

  set_sda (1);		// tristate SDA
  set_scl (1);
  ack_bit = get_sda ();	// slave should pull SDA line low
  set_scl (0);

  return ack_bit == 0;
}


// write: the high level entry point...
//	entry: SCL = 1, SDA = 1
//	exit:  SCL = 1, SDA = 1

bool
i2c_bitbang::write (int addr, const unsigned char *buf, int nbytes)
{
  bool	ok = true;
  
  d_io->lock ();
  start ();
  ok = write_byte ((addr << 1) | 0);	// addr plus "read opcode"

  for (int i = 0; i < nbytes; i++)
    ok &= write_byte (buf[i]);

  stop ();
  d_io->unlock ();
  return ok;
}


// read: the high level entry point...
//	entry: SCL = 1, SDA = 1
//	exit:  SCL = 1, SDA = 1

int
i2c_bitbang::read (int addr, unsigned char *buf, int max_bytes)
{
  d_io->lock ();

  // FIXME

  d_io->unlock ();
  return -1;
}
