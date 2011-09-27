/*---------------------------------------------------------------------------*\

  FILE........: c2enc.c
  AUTHOR......: David Rowe
  DATE CREATED: 23/8/2010

  Encodes a file of raw speech samples using codec2 and outputs a file
  of bits.

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define BITS_SIZE	((CODEC2_BITS_PER_FRAME + 7) / 8)

int main(int argc, char *argv[])
{
    void *codec2;
    FILE *fin;
    FILE *fout;
    short buf[CODEC2_SAMPLES_PER_FRAME];
    unsigned char  bits[BITS_SIZE];

    if (argc != 3) {
	printf("usage: %s InputRawspeechFile OutputBitFile\n", argv[0]);
	exit(1);
    }
 
    if (strcmp(argv[1], "-")  == 0) fin = stdin;
    else if ( (fin = fopen(argv[1],"rb")) == NULL ) {
	fprintf(stderr, "Error opening input bit file: %s: %s.\n",
         argv[1], strerror(errno));
	exit(1);
    }

    if (strcmp(argv[2], "-") == 0) fout = stdout;
    else if ( (fout = fopen(argv[2],"wb")) == NULL ) {
	fprintf(stderr, "Error opening output speech file: %s: %s.\n",
         argv[2], strerror(errno));
	exit(1);
    }

    codec2 = codec2_create();

    while(fread(buf, sizeof(short), CODEC2_SAMPLES_PER_FRAME, fin) ==
	  CODEC2_SAMPLES_PER_FRAME) {
	codec2_encode(codec2, bits, buf);
	fwrite(bits, sizeof(char), BITS_SIZE, fout);
	//if this is in a pipeline, we probably don't want the usual
        //buffering to occur
        if (fout == stdout) fflush(stdout);
        if (fin == stdin) fflush(stdin);
    }

    codec2_destroy(codec2);

    fclose(fin);
    fclose(fout);

    return 0;
}
