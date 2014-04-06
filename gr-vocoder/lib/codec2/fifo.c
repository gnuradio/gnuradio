/*---------------------------------------------------------------------------*\
                                                                             
  FILE........: fifo.c
  AUTHOR......: David Rowe
  DATE CREATED: Oct 15 2012
                                                                             
  A FIFO design useful in gluing the FDMDV modem and codec together in
  integrated applications.  The unittest/tfifo indicates these
  routines are thread safe without the need for syncronisation
  object, e.g. a different thread can read and write to a fifo at the
  same time.

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

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "codec2_fifo.h"

struct FIFO {
    short *buf;
    short *pin;
    short *pout;
    int    nshort;
};

struct FIFO *fifo_create(int nshort) {
    struct FIFO *fifo;

    fifo = (struct FIFO *)malloc(sizeof(struct FIFO));
    assert(fifo != NULL);

    fifo->buf = (short*)malloc(sizeof(short)*nshort);
    assert(fifo->buf != NULL);
    fifo->pin = fifo->buf;
    fifo->pout = fifo->buf;
    fifo->nshort = nshort;

    return fifo;
}

void fifo_destroy(struct FIFO *fifo) {
    assert(fifo != NULL);
    free(fifo->buf);
    free(fifo);
}

int fifo_write(struct FIFO *fifo, short data[], int n) {
    int            i;
    int            fifo_free;
    short         *pdata;
    short         *pin = fifo->pin;

    assert(fifo != NULL);
    assert(data != NULL);

    // available storage is one less than nshort as prd == pwr
    // is reserved for empty rather than full

    fifo_free = fifo->nshort - fifo_used(fifo) - 1;

    if (n > fifo_free) {
	return -1;
    }
    else {

	/* This could be made more efficient with block copies
	   using memcpy */

	pdata = data;
	for(i=0; i<n; i++) {
	    *pin++ = *pdata++;
	    if (pin == (fifo->buf + fifo->nshort))
		pin = fifo->buf;
	}
	fifo->pin = pin;
    }

    return 0;
}

int fifo_read(struct FIFO *fifo, short data[], int n)
{
    int            i;
    short         *pdata;
    short         *pout = fifo->pout;

    assert(fifo != NULL);
    assert(data != NULL);
 
    if (n > fifo_used(fifo)) {
	return -1;
    }
    else {

	/* This could be made more efficient with block copies
	   using memcpy */

	pdata = data;
	for(i=0; i<n; i++) {
	    *pdata++ = *pout++;
	    if (pout == (fifo->buf + fifo->nshort))
		pout = fifo->buf;
	}
	fifo->pout = pout;
    }

    return 0;
}

int fifo_used(struct FIFO *fifo)
{
    short         *pin = fifo->pin;
    short         *pout = fifo->pout;
    unsigned int   used;

    assert(fifo != NULL);
    if (pin >= pout)
        used = pin - pout;
    else
        used = fifo->nshort + (unsigned int)(pin - pout);

    return used;
}

