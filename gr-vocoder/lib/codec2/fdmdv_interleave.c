/*---------------------------------------------------------------------------*\
                                                                             
  FILE........: fdmdv_interleave.c
  AUTHOR......: David Rowe  
  DATE CREATED: May 27 2012
                                                                             
  Given an input file of bits outputs an interleaved or optionally
  de-intervleaved file of bits.
                                                                             
\*---------------------------------------------------------------------------*/


/*
  Copyright (C) 2012 David Rowe

  All rights reserved.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License version 2, as
  published by the Free Software Foundation.  This program is
  distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
  License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program; if not, see <http://www.gnu.org/licenses/>.
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#include "codec2_fdmdv.h"

#define MAX_INTERLEAVER 10000

int main(int argc, char *argv[])
{
    FILE         *fin, *fout, *finter;
    int          interleaver[MAX_INTERLEAVER];
    char         *packed_bits;
    int          *bits;
    int          *interleaved_bits;
    int           i, bit, byte, m, mpacked, frames, interleave, src_bit, dest_bit;

    if (argc < 4) {
	printf("usage: %s InputBitFile OutputBitFile InterleaverFile [de]\n", argv[0]);
	printf("e.g    %s hts1a.c2 hts1a_interleaved.c2 interleaver.txt\n", argv[0]);
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
	fprintf(stderr, "Error opening output bit file: %s: %s.\n",
         argv[2], strerror(errno));
	exit(1);
    }

    if ((finter = fopen(argv[3],"rt")) == NULL ) {
	fprintf(stderr, "Error opening interleaver file: %s: %s.\n",
         argv[3], strerror(errno));
	exit(1);
    }

    if (argc == 5)
	interleave = 1;
    else
	interleave = 0;

    /* load interleaver, size determines block size we will process */

    src_bit = 0;
    while(fscanf(finter, "%d\n", &dest_bit) == 1) {
	if (interleave)
	    interleaver[dest_bit] = src_bit;
	else
	    interleaver[src_bit] = dest_bit;
	    
	src_bit++;
	if (src_bit == MAX_INTERLEAVER) {
	    fprintf(stderr, "Error interleaver too big\n");
	    exit(1);
	}	
    }
    fclose(finter);

    m = src_bit;
    fprintf(stderr, "Interleaver size m = %d  interleave = %d\n", m, interleave);
    assert((m%8) == 0);
    mpacked = m/8;

    packed_bits = (char*)malloc(mpacked*sizeof(char));
    assert(packed_bits != NULL);
    bits = (int*)malloc(m*sizeof(int));
    assert(bits != NULL);
    interleaved_bits = (int*)malloc(m*sizeof(int));
    assert(interleaved_bits != NULL);

    frames = 0;

    while(fread(packed_bits, sizeof(char), mpacked, fin) == mpacked) {
	frames++;
	
	/* unpack bits, MSB first */

	bit = 7; byte = 0;
	for(i=0; i<m; i++) {
	    bits[i] = (packed_bits[byte] >> bit) & 0x1;
	    bit--;
	    if (bit < 0) {
		bit = 7;
		byte++;
	    }
	}
	assert(byte == mpacked);

	/* (de) interleave */

	for(i=0; i<m; i++)
	    interleaved_bits[i] = bits[interleaver[i]];

	/* pack bits, MSB sent first  */

	bit = 7; byte = 0;
	memset(packed_bits, 0, mpacked);
	for(i=0; i<m; i++) {
	    packed_bits[byte] |= (interleaved_bits[i] << bit);
	    bit--;
	    if (bit < 0) {
		bit = 7;
		byte++;
	    }
	}
	assert(byte == mpacked);

	fwrite(packed_bits, sizeof(char), mpacked, fout);

	/* if this is in a pipeline, we probably don't want the usual
	   buffering to occur */

        if (fout == stdout) fflush(stdout);
        if (fin == stdin) fflush(stdin);         
    }

    free(packed_bits);
    free(bits);
    free(interleaved_bits);

    fclose(fin);
    fclose(fout);

    return 0;
}
