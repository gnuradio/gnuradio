/*
 * avrdude - A Downloader/Uploader for AVR device programmers
 * Copyright (C) 2003-2004  Theodore A. Roth  <troth@openavr.org>
 * Copyright (C) 2005 Johnathan Corgan <jcorgan@aeinet.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
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

#ifndef __ftbb_h__
#define __ftbb_h__

#include "avrpart.h"

void ftbb_initpgm (PROGRAMMER *pgm);

// TODO: Update with actual id's once established for product
// These are for FTDI unprogrammed parts
#define EZDOP_VENDORID  0x0403
#define EZDOP_PRODUCTID 0x6001

#endif /* __ftbb_h__ */
