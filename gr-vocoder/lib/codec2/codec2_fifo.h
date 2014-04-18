/*---------------------------------------------------------------------------*\
                                                                             
  FILE........: codec2_fifo.h
  AUTHOR......: David Rowe
  DATE CREATED: Oct 15 2012
                                                                             
  A FIFO design useful in gluing the FDMDV modem and codec together in
  integrated applications.
  
  The name codec2_fifo.h is used to make it unique when "make
  installed".                   

\*---------------------------------------------------------------------------*/

/*
  Copyright (C) 2012 David Rowe

  All rights reserved.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License version 2.1, as
  published by the Free Software Foundation.  This program is
  distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
  License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program; if not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __FIFO__
#define __FIFO__

#ifdef __cplusplus
extern "C" {
#endif

struct FIFO;

struct FIFO *fifo_create(int nshort);
void fifo_destroy(struct FIFO *fifo);
int fifo_write(struct FIFO *fifo, short data[], int n);
int fifo_read(struct FIFO *fifo, short data[], int n);
int fifo_used(struct FIFO *fifo);

#ifdef __cplusplus
}
#endif

#endif
