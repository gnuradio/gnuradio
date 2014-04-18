/*---------------------------------------------------------------------------*\
                                                                             
  FILE........: fdmdv_put_test_bits.c
  AUTHOR......: David Rowe  
  DATE CREATED: 1 May 2012
                                                                             
  Using a file of packed test bits as input, determines bit error
  rate.  Useful for testing fdmdv_demod.

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
    FILE         *fin;
    struct FDMDV *fdmdv;
    char         *packed_bits;
    int          *rx_bits;
    int           i, bit, byte;
    int           test_frame_sync, bit_errors, total_bit_errors, total_bits, ntest_bits;
    int           test_frame_sync_state, test_frame_count;
    int           bits_per_fdmdv_frame;
    int           bits_per_codec_frame;
    int           bytes_per_codec_frame;
    int           Nc;
    short        *error_pattern;

    if (argc < 2) {
	printf("usage: %s InputBitFile [Nc]\n", argv[0]);
	printf("e.g    %s test.c2\n", argv[0]);
	exit(1);
    }

    if (strcmp(argv[1], "-") == 0) fin = stdin;
    else if ( (fin = fopen(argv[1],"rb")) == NULL ) {
	fprintf(stderr, "Error opening input bit file: %s: %s.\n",
         argv[1], strerror(errno));
	exit(1);
    }

    if (argc == 3) {
        Nc = atoi(argv[2]);
        if ((Nc % 2) != 0) {
            fprintf(stderr, "Error number of carriers must be a multiple of 2\n");
            exit(1);
        }
        if ((Nc < 2) || (Nc > FDMDV_NC_MAX) ) {
            fprintf(stderr, "Error number of carriers must be between 2 and %d\n",  FDMDV_NC_MAX);
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
    rx_bits = (int*)malloc(sizeof(int)*bits_per_codec_frame);
    assert(rx_bits != NULL);

    error_pattern = (short*)malloc(fdmdv_error_pattern_size(fdmdv)*sizeof(int));
    assert(error_pattern != NULL);

    total_bit_errors = 0;
    total_bits = 0;
    test_frame_sync_state = 0;
    test_frame_count = 0;

    while(fread(packed_bits, sizeof(char), bytes_per_codec_frame, fin) == bytes_per_codec_frame) {
	/* unpack bits, MSB first */

	bit = 7; byte = 0;
	for(i=0; i<bits_per_codec_frame; i++) {
	    rx_bits[i] = (packed_bits[byte] >> bit) & 0x1;
	    //printf("%d 0x%x %d\n", i, packed_bits[byte], rx_bits[i]);
	    bit--;
	    if (bit < 0) {
		bit = 7;
		byte++;
	    }
	}
	assert(byte == bytes_per_codec_frame);

	fdmdv_put_test_bits(fdmdv, &test_frame_sync, error_pattern, &bit_errors, &ntest_bits, rx_bits);

	if (test_frame_sync == 1) {
	    test_frame_sync_state = 1;
            test_frame_count = 0;
        }

        if (test_frame_sync_state) {
            if (test_frame_count == 0) {
                total_bit_errors += bit_errors;
                total_bits = total_bits + ntest_bits;
                printf("+");
            }
            else
                printf("-");
            test_frame_count++;
            if (test_frame_count == 4)
                test_frame_count = 0;
	}
        else
            printf("-");

  	fdmdv_put_test_bits(fdmdv, &test_frame_sync,  error_pattern, &bit_errors, &ntest_bits, &rx_bits[bits_per_fdmdv_frame]);

	if (test_frame_sync == 1) {
	    test_frame_sync_state = 1;
            test_frame_count = 0;
        }

        if (test_frame_sync_state) {
            if (test_frame_count == 0) {
                total_bit_errors += bit_errors;
                total_bits = total_bits + ntest_bits;
                printf("+");
            }
            else
                printf("-");
            test_frame_count++;
            if (test_frame_count == 4)
                test_frame_count = 0;
	}
	else
	    printf("-");
	
	/* if this is in a pipeline, we probably don't want the usual
	   buffering to occur */

        if (fin == stdin) fflush(stdin);
    }

    fclose(fin);
    free(error_pattern);
    fdmdv_destroy(fdmdv);

    printf("\nbits %d  errors %d  BER %1.4f\n", total_bits, total_bit_errors, (float)total_bit_errors/(1E-6+total_bits) );
    return 0;
}

