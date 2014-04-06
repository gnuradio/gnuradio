/*---------------------------------------------------------------------------*\

  FILE........: c2dec.c
  AUTHOR......: David Rowe
  DATE CREATED: 23/8/2010

  Decodes a file of bits to a file of raw speech samples using codec2.

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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define NONE          0  /* no bit errors                          */
#define UNIFORM       1  /* random bit errors                      */
#define TWO_STATE     2  /* Two state error model                  */
#define UNIFORM_RANGE 3  /* random bit errors over a certain range */

int main(int argc, char *argv[])
{
    int            mode;
    void          *codec2;
    FILE          *fin;
    FILE          *fout;
    FILE          *fber = NULL;
    short         *buf;
    unsigned char *bits, *prev_bits;
    int            nsam, nbit, nbyte, i, byte, frames, bits_proc, bit_errors, error_mode;
    int            nstart_bit, nend_bit, bit_rate;
    int            state, next_state;
    float          ber, r, burst_length, burst_period, burst_timer, ber_est;
    unsigned char  mask;

    if ((argc != 4) && (argc != 5) && (argc != 6) && (argc != 7)) {
	printf("basic usage.................: c2dec 3200|2400|1600|1400|1300|1200 InputBitFile OutputRawSpeechFile\n");
	printf("uniform errors usage........: c2dec 3200|2400|1600|1400|1300|1200 InputBitFile OutputRawSpeechFile uniformBER startBit endBit\n");
	printf("uniform error on range usage: c2dec 3200|2400|1600|1400|1300|1200 InputBitFile OutputRawSpeechFile uniformBER\n");
	printf("demod BER estimate..........: c2dec 3200|2400|1600|1400|1300|1200 InputBitFile OutputRawSpeechFile BERfile\n");
	printf("two state fading usage......: c2dec 3200|2400|1600|1400|1300|1200 InputBitFile OutputRawSpeechFile burstLength burstPeriod\n");
	printf("e.g    c2dec 1400 hts1a.c2 hts1a_1400.raw\n");
	printf("e.g    c2dec 1400 hts1a.c2 hts1a_1400.raw 0.9\n");
	printf("e.g    c2dec 1400 hts1a.c2 hts1a_1400.raw 0.99 0.9\n");
	exit(1);
    }

    if (strcmp(argv[1],"3200") == 0)
	mode = CODEC2_MODE_3200;
    else if (strcmp(argv[1],"2400") == 0)
	mode = CODEC2_MODE_2400;
    else if (strcmp(argv[1],"1600") == 0)
	mode = CODEC2_MODE_1600;
    else if (strcmp(argv[1],"1400") == 0)
	mode = CODEC2_MODE_1400;
    else if (strcmp(argv[1],"1300") == 0)
	mode = CODEC2_MODE_1300;
    else if (strcmp(argv[1],"1200") == 0)
	mode = CODEC2_MODE_1200;
    else {
	fprintf(stderr, "Error in mode: %s.  Must be 3200, 2400, 1600, 1400, 1300 or 1200\n", argv[1]);
	exit(1);
    }
    bit_rate = atoi(argv[1]);

    if (strcmp(argv[2], "-")  == 0) fin = stdin;
    else if ( (fin = fopen(argv[2],"rb")) == NULL ) {
	fprintf(stderr, "Error opening input bit file: %s: %s.\n",
         argv[2], strerror(errno));
	exit(1);
    }

    if (strcmp(argv[3], "-") == 0) fout = stdout;
    else if ( (fout = fopen(argv[3],"wb")) == NULL ) {
	fprintf(stderr, "Error opening output speech file: %s: %s.\n",
         argv[3], strerror(errno));
	exit(1);
    }

    error_mode = NONE;
    ber = 0.0;
    burst_length = burst_period = 0.0;
    burst_timer = 0.0;

    codec2 = codec2_create(mode);
    nsam = codec2_samples_per_frame(codec2);
    nbit = codec2_bits_per_frame(codec2);
    buf = (short*)malloc(nsam*sizeof(short));
    nbyte = (nbit + 7) / 8;
    bits = (unsigned char*)malloc(nbyte*sizeof(char));
    prev_bits = (unsigned char*)malloc(nbyte*sizeof(char));
    frames = bit_errors = bits_proc = 0;
    nstart_bit = 0;
    nend_bit = nbit-1;

    if (argc == 5) {
        /* see if 4th argument is a valid file name */
        if ( (fber = fopen(argv[4],"rb")) == NULL ) {
            /* otherwise it must be BER value for uniform errors */
            ber = atof(argv[4]);
	    error_mode = UNIFORM;
        }
    }

    if (argc == 6) {
        error_mode = TWO_STATE;
	burst_length = atof(argv[4]);
	burst_period = atof(argv[5]);
	nstart_bit = 0;
	nend_bit = 2;
        state = 0;
    }
    
    if (argc == 7) {
        error_mode = UNIFORM_RANGE;
	ber = atof(argv[4]);
	nstart_bit = atoi(argv[5]);
	nend_bit = atoi(argv[6]);
        fprintf(stderr, "ber: %f nstart_bit: %d nend_bit: %d\n", ber, nstart_bit, nend_bit);
        state = 0;
    }

    assert(nend_bit <= nbit);

    while(fread(bits, sizeof(char), nbyte, fin) == (size_t)nbyte) {
	frames++;

        // apply bit errors, MSB of byte 0 is bit 0 in frame */
        
	if ((error_mode == UNIFORM) || (error_mode == UNIFORM_RANGE)) {
	    for(i=nstart_bit; i<nend_bit+1; i++) {
		r = (float)rand()/RAND_MAX;
		if (r < ber) {
		    byte = i/8;
		    //printf("nbyte %d nbit %d i %d byte %d bits[%d] 0x%0x ", nbyte, nbit, i, byte, byte, bits[byte]);
		    mask = 1 << (7 - i + byte*8);
                    bits[byte] ^= mask;
		    //printf("shift: %d mask: 0x%0x bits[%d] 0x%0x\n", 7 - i + byte*8, mask, byte, bits[byte] );
		    bit_errors++;
 		}
                bits_proc++;
	    }
	}

	if (error_mode == TWO_STATE) {
            burst_timer += (float)nbit/bit_rate;
            fprintf(stderr, "burst_timer: %f  state: %d\n", burst_timer, state);

            next_state = state;
            switch(state) {
            case 0:

                /* clear channel state - no bit errors */

                if (burst_timer > (burst_period - burst_length))
                    next_state = 1;
                break;

            case 1:
                
                /* burst error state - 50% bit error rate */

                for(i=nstart_bit; i<nend_bit+1; i++) {
                    r = (float)rand()/RAND_MAX;
                    if (r < 0.5) {
                        byte = i/8;
                        bits[byte] ^= 1 << (7 - i + byte*8);
                        bit_errors++;
                    }
                    bits_proc++;
		}

                if (burst_timer > burst_period) {
                    burst_timer = 0.0;
                    next_state = 0;
                }
                break;

	    }
               
            state = next_state;
        }

        if (fber != NULL) {
            if (fread(&ber_est, sizeof(float), 1, fber) != 1) {
                fprintf(stderr, "ran out of BER estimates!\n");
                exit(1);
            }
            //fprintf(stderr, "ber_est: %f\n", ber_est);
        }
        else
            ber_est = 0.0;

        /* frame repeat logic */
        if (ber_est > 0.15) {
            //memcpy(bits, prev_bits, nbyte);
            // fprintf(stderr, "repeat\n");
        }
            
	codec2_decode(codec2, buf, bits, ber_est);
 	fwrite(buf, sizeof(short), nsam, fout);
	//if this is in a pipeline, we probably don't want the usual
        //buffering to occur
        if (fout == stdout) fflush(stdout);
        if (fin == stdin) fflush(stdin);         

        memcpy(prev_bits, bits, nbyte);
    }

    if (error_mode)
	fprintf(stderr, "actual BER: %1.3f\n", (float)bit_errors/bits_proc);

    codec2_destroy(codec2);

    free(buf);
    free(bits);
    fclose(fin);
    fclose(fout);

    return 0;
}
