/*---------------------------------------------------------------------------*\

  FILE........: c2demo.c
  AUTHOR......: David Rowe
  DATE CREATED: 15/11/2010

  Encodes and decodes a file of raw speech samples using Codec 2.
  Demonstrates use of Codec 2 function API.

  Note to convert a wave file to raw and vice-versa:

    $ sox file.wav -r 8000 -s -2 file.raw
    $ sox -r 8000 -s -2 file.raw file.wav

\*---------------------------------------------------------------------------*/

/*
  Copyright (C) 2010 David Rowe

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

#include "codec2.h"
#include "sine.h"
#include "dump.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    struct CODEC2 *codec2;
    FILE          *fin;
    FILE          *fout;
    short         *buf;
    unsigned char *bits;
    int            nsam, nbit, i, r;

    for(i=0; i<10; i++) {
        r = codec2_rand();
        printf("[%d] r = %d\n", i, r);
    }

    if (argc != 3) {
	printf("usage: %s InputRawSpeechFile OutputRawSpeechFile\n", argv[0]);
	exit(1);
    }

    if ( (fin = fopen(argv[1],"rb")) == NULL ) {
	fprintf(stderr, "Error opening input speech file: %s: %s.\n",
         argv[1], strerror(errno));
	exit(1);
    }

    if ( (fout = fopen(argv[2],"wb")) == NULL ) {
	fprintf(stderr, "Error opening output speech file: %s: %s.\n",
         argv[2], strerror(errno));
	exit(1);
    }

    #ifdef DUMP
    dump_on("c2demo");
    #endif

    /* Note only one set of Codec 2 states is required for an encoder
       and decoder pair. */

    codec2 = codec2_create(CODEC2_MODE_1300);
    nsam = codec2_samples_per_frame(codec2);
    buf = (short*)malloc(nsam*sizeof(short));
    nbit = codec2_bits_per_frame(codec2);
    bits = (unsigned char*)malloc(nbit*sizeof(char));

    while(fread(buf, sizeof(short), nsam, fin) == (size_t)nsam) {
	codec2_encode(codec2, bits, buf);
	codec2_decode(codec2, buf, bits);
	fwrite(buf, sizeof(short), nsam, fout);
    }

    free(buf);
    free(bits);
    codec2_destroy(codec2);

    fclose(fin);
    fclose(fout);

    return 0;
}
