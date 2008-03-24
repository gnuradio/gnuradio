/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <spu_buffers.h>
#include <compiler.h>

static unsigned char _getbuf[NGETBUFS][GC_SPU_BUFSIZE] _AL128;
static unsigned char _putbuf[NPUTBUFS][GC_SPU_BUFSIZE] _AL128;

unsigned char *_gci_getbuf[NGETBUFS] = {
  _getbuf[0]
};

unsigned char *_gci_putbuf[NPUTBUFS] = {
  _putbuf[0],
  _putbuf[1]
};
