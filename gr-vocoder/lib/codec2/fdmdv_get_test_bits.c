/*---------------------------------------------------------------------------*\
                                                                             
  FILE........: fdmdv_get_test_bits.c
  AUTHOR......: David Rowe  
  DATE CREATED: 1 May 2012
                                                                             
  Generates a file of packed test bits, useful for input to fdmdv_mod.

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

int main(int argc, char *argv[])
{
    FILE         *fout;
    struct FDMDV *fdmdv;
    char          *packed_bits;
    int           *tx_bits;
    int           n, i, bit, byte;
    int           numBits, nCodecFrames;
    int           bits_per_fdmdv_frame;
    int           bits_per_codec_frame;
    int           bytes_per_codec_frame;
    int           Nc;

    if (argc < 3) {
	printf("usage: %s OutputBitFile numBits [Nc]\n", argv[0]);
	printf("e.g    %s test.c2 1400\n", argv[0]);
	exit(1);
    }

    if (strcmp(argv[1], "-") == 0) fout = stdout;
    else if ( (fout = fopen(argv[1],"wb")) == NULL ) {
	fprintf(stderr, "Error opening output bit file: %s: %s.\n",
         argv[1], strerror(errno));
	exit(1);
    }

    numBits = atoi(argv[2]);

    if (argc == 4) {
        Nc = atoi(argv[3]);
        if ((Nc % 2) != 0) {
            fprintf(stderr, "Error number of carriers must be a multiple of 2\n");
            exit(1);
        }
        if ((Nc < 2) || (Nc > FDMDV_NC_MAX) ) {
            fprintf(stderr, "Error number of carriers must be btween 2 and %d\n",  FDMDV_NC_MAX);
            exit(1);
        }
    }
    else
        Nc = FDMDV_NC;

    fdmdv = fdmdv_create(Nc);

    bits_per_fdmdv_frame = fdmdv_bits_per_frame(fdmdv);
    bits_per_codec_frame = 2*fdmdv_bits_per_frame(fdmdv);
    assert((bits_per_codec_frame % 8) == 0); /* make sure integer number of bytes per frame */
    bytes_per_codec_frame = bits_per_codec_frame/8;
    fprintf(stderr, "bits_per_fdmdv_frame: %d bits_per_codec_frame: %d bytes_per_codec_frame: %d\n",
            bits_per_fdmdv_frame, bits_per_codec_frame, bytes_per_codec_frame);

    packed_bits = (char*)malloc(bytes_per_codec_frame);
    assert(packed_bits != NULL);
    tx_bits = (int*)malloc(sizeof(int)*bits_per_codec_frame);
    assert(tx_bits != NULL);

    nCodecFrames = numBits/bits_per_codec_frame;

    for(n=0; n<nCodecFrames; n++) {

	fdmdv_get_test_bits(fdmdv, tx_bits);
	fdmdv_get_test_bits(fdmdv, &tx_bits[bits_per_fdmdv_frame]);
	
	/* pack bits, MSB received first  */

	bit = 7; byte = 0;
	memset(packed_bits, 0, bytes_per_codec_frame);
	for(i=0; i<bits_per_codec_frame; i++) {
	    packed_bits[byte] |= (tx_bits[i] << bit);
	    bit--;
	    if (bit < 0) {
		bit = 7;
		byte++;
	    }
	}
	assert(byte == bytes_per_codec_frame);

	fwrite(packed_bits, sizeof(char), bytes_per_codec_frame, fout);
 
	/* if this is in a pipeline, we probably don't want the usual
	   buffering to occur */

        if (fout == stdout) fflush(stdout);
    }

    free(tx_bits);
    free(packed_bits);
    fclose(fout);
    fdmdv_destroy(fdmdv);

    return 0;
}

