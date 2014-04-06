/*---------------------------------------------------------------------------*\
                                                                             
  FILE........: fdmdv_mod.c
  AUTHOR......: David Rowe  
  DATE CREATED: April 28 2012
                                                                             
  Given an input file of bits outputs a raw file (8kHz, 16 bit shorts)
  of FDMDV modem samples ready to send over a HF radio channel.  The
  input file is assumed to be arranged as codec frames of 56 bits (7
  bytes) which we send as two 28 bit modem frames.
                                                                             
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
    FILE         *fin, *fout;
    struct FDMDV *fdmdv;
    char          *packed_bits;
    int           *tx_bits;
    COMP          tx_fdm[2*FDMDV_NOM_SAMPLES_PER_FRAME];
    short         tx_fdm_scaled[2*FDMDV_NOM_SAMPLES_PER_FRAME];
    int           frames;
    int           i, bit, byte;
    int           sync_bit;
    int           bits_per_fdmdv_frame;
    int           bits_per_codec_frame;
    int           bytes_per_codec_frame;
    int           Nc;

    if (argc < 3) {
	printf("usage: %s InputBitFile OutputModemRawFile [Nc]\n", argv[0]);
	printf("e.g    %s hts1a.c2 hts1a_fdmdv.raw\n", argv[0]);
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
	fprintf(stderr, "Error opening output modem sample file: %s: %s.\n",
         argv[2], strerror(errno));
	exit(1);
    }

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

    packed_bits = (char*)malloc(bytes_per_codec_frame);
    assert(packed_bits != NULL);
    tx_bits = (int*)malloc(sizeof(int)*bits_per_codec_frame);
    assert(tx_bits != NULL);

    frames = 0;

    while(fread(packed_bits, sizeof(char), bytes_per_codec_frame, fin) == bytes_per_codec_frame) {
	frames++;
	
	/* unpack bits, MSB first */

	bit = 7; byte = 0;
	for(i=0; i<bits_per_codec_frame; i++) {
	    tx_bits[i] = (packed_bits[byte] >> bit) & 0x1;
	    bit--;
	    if (bit < 0) {
		bit = 7;
		byte++;
	    }
	}
	assert(byte == bytes_per_codec_frame);

	/* modulate even and odd frames */

	fdmdv_mod(fdmdv, tx_fdm, tx_bits, &sync_bit);
	assert(sync_bit == 1);

	fdmdv_mod(fdmdv, &tx_fdm[FDMDV_NOM_SAMPLES_PER_FRAME], &tx_bits[bits_per_fdmdv_frame], &sync_bit);
	assert(sync_bit == 0);

	/* scale and save to disk as shorts */

	for(i=0; i<2*FDMDV_NOM_SAMPLES_PER_FRAME; i++)
	    tx_fdm_scaled[i] = FDMDV_SCALE * tx_fdm[i].real;

 	fwrite(tx_fdm_scaled, sizeof(short), 2*FDMDV_NOM_SAMPLES_PER_FRAME, fout);

	/* if this is in a pipeline, we probably don't want the usual
	   buffering to occur */

        if (fout == stdout) fflush(stdout);
        if (fin == stdin) fflush(stdin);         
    }

    //fdmdv_dump_osc_mags(fdmdv);

    free(tx_bits);
    free(packed_bits);
    fclose(fin);
    fclose(fout);
    fdmdv_destroy(fdmdv);

    return 0;
}
